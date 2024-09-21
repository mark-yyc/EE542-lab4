#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define FILE_NAME "output.txt"
#define FILE_SIZE (1L * 1024 * 1024 * 1024)
#define PAGE_SIZE 1024
#define NUM_THREADS_RECEIVER 64
#define REPLY_SIZE 4 + sizeof(double)

unsigned char ACK[FILE_SIZE / PAGE_SIZE];
int ACK_num = 0;
int server_sockfd, client_sockfd;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
pthread_mutex_t mutex;

void printTimestamp()
{
    time_t current_time;
    time(&current_time);
    char *timestamp = ctime(&current_time);
    printf("File received, current timestamp: %s", timestamp);
}

void *thread_revfile(void *arg)
{
    int sent_seq, thread_id = *(int *)arg;
    char reply[REPLY_SIZE];
    char buffer[PAGE_SIZE + sizeof(int)];
    socklen_t len = sizeof(client_addr);

    FILE *file = fopen(FILE_NAME, "r+b");
    if (file == NULL)
    {
        file = fopen(FILE_NAME, "wb+");
        if (file == NULL)
        {
            perror("Error opening file");
            exit(1);
        }
    }

    while (1)
    {
        int n = recvfrom(server_sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &len);
        if (strncmp(buffer, "RTT:", 4) == 0)
        {
            memcpy(reply, buffer, REPLY_SIZE);
            sendto(server_sockfd, reply, sizeof(reply), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
            continue;
        }
        int received_seq;
        memcpy(&received_seq, buffer, sizeof(int));
        if (ACK[received_seq] == 0)
        {
            ACK[received_seq] = 1;
            // printf("server thread %d: received file seq: %d\n", thread_id, received_seq);
            pthread_mutex_lock(&mutex);
            ACK_num++;
            pthread_mutex_unlock(&mutex);
            if (ACK_num * PAGE_SIZE == FILE_SIZE)
            {
                printTimestamp();
            }
            if (fseek(file, received_seq * PAGE_SIZE, SEEK_SET) != 0)
            {
                perror("Error seeking to offset");
                fclose(file);
                exit(1);
            }
            fwrite(buffer + sizeof(int), 1, PAGE_SIZE, file);
        }
        memcpy(reply, "ACK:", 4);
        memcpy(reply + 4, &received_seq, sizeof(int));
        sendto(server_sockfd, reply, sizeof(reply), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    memset(ACK, 0, sizeof(ACK));
    int ret;
    pthread_t receiver[NUM_THREADS_RECEIVER];
    pthread_mutex_init(&mutex, NULL);
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    ret = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (-1 == ret)
    {
        perror("bind");
        exit(1);
    }

    for (int i = 0; i < NUM_THREADS_RECEIVER; i++)
    {
        pthread_create(&receiver[i], NULL, thread_revfile, &i);
    }
    for (int i = 0; i < NUM_THREADS_RECEIVER; i++)
    {
        pthread_join(receiver[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    close(server_sockfd);
    return 0;
}