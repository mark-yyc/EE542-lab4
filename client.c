#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define FILE_SIZE (1L * 1024 * 1024 * 1024)
#define FILE_NAME "file.txt"
#define PAGE_SIZE 1024
#define NUM_THREADS_SENDER 64
#define NUM_THREADS_RECEIVER 64
#define NUM_THREADS_RTT 8
#define REPLY_SIZE 4 + sizeof(double)

typedef struct
{
    int thread_id;
    long offset;
    long chunk_size;
} ThreadData;

int sockfd;
double RTT;
struct sockaddr_in server_addr;
unsigned char ACK[FILE_SIZE / PAGE_SIZE];
unsigned char timeFlag = 0;

void printTimestamp()
{
    time_t current_time;
    time(&current_time);
    char *timestamp = ctime(&current_time);
    printf("File sending, current timestamp: %s", timestamp);
}

void *thread_sendfile(void *arg)
{
    int packetIndex, sequenceNumber;
    ThreadData *data = (ThreadData *)arg;
    char buffer[PAGE_SIZE];
    char packet[PAGE_SIZE + sizeof(int)];
    int packetNumber = data->chunk_size / PAGE_SIZE;
    int beginSequenceNumber = data->thread_id * packetNumber;
    unsigned char allSent = 0;
    size_t bytesRead;
    long beginPosition;

    FILE *file = fopen(FILE_NAME, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    fseek(file, data->offset, SEEK_SET);
    beginPosition = ftell(file);

    while (allSent == 0)
    {
        allSent = 1;
        for (packetIndex = 0; packetIndex < packetNumber; packetIndex++)
        {
            sequenceNumber = packetIndex + beginSequenceNumber;
            if (ACK[sequenceNumber] == 0)
            {
                allSent = 0;
                fseek(file, beginPosition + packetIndex * PAGE_SIZE, SEEK_SET);
                bytesRead = fread(buffer, 1, PAGE_SIZE, file);
                if (bytesRead > 0)
                {
                    memset(packet, 0, sizeof(packet));
                    memcpy(packet, &sequenceNumber, sizeof(int));
                    memcpy(packet + sizeof(int), buffer, bytesRead);
                    sendto(sockfd,
                           packet,
                           bytesRead + sizeof(int),
                           0,
                           (struct sockaddr *)&server_addr,
                           sizeof(server_addr));
                    // printf("Sender thread %d: sending file seq: %d\n", data->thread_id, sequenceNumber);
                }
            }
        }
        sleep(2L * RTT);
        if (allSent == 1)
        {
            printf("Sender thread %d: exit\n", data->thread_id);
        }
    }

    fclose(file);

    pthread_exit(NULL);
}

void *thread_revfile(void *arg)
{
    int sent_seq;
    int thread_id = *(int *)arg;
    double begin;
    struct timespec end;
    char reply[REPLY_SIZE];
    while (1)
    {
        int n = recvfrom(sockfd, reply, sizeof(reply), 0, NULL, NULL);
        if (strncmp(reply, "RTT:", 4) == 0)
        {
            memcpy(&begin, reply + 4, sizeof(double));
            if (clock_gettime(CLOCK_REALTIME, &end) == -1)
            {
                perror("clock gettime");
            }
            RTT = end.tv_sec + (double)end.tv_nsec / 1e9 - begin;
        }
        else
        {
            memcpy(&sent_seq, reply + 4, sizeof(int));
            // printf("Receiver %d: received file seq: %d\n", thread_id, sent_seq);
            if (ACK[sent_seq] == 0)
            {
                ACK[sent_seq] = 1;
            }
        }
    }
    pthread_exit(NULL);
}

void *thread_RTT(void *arg)
{
    struct timespec start;
    double time;
    char packet[PAGE_SIZE + sizeof(int)];
    while (RTT < 0)
    {
        if (clock_gettime(CLOCK_REALTIME, &start) == -1)
        {
            perror("clock gettime");
        }
        time = start.tv_sec + (double)start.tv_nsec / 1e9;
        memset(packet, 0, sizeof(packet));
        memcpy(packet, "RTT:", 4);
        memcpy(packet + 4, &time, sizeof(double));
        // printf("Sending RTT request: %s\n", packet);
        sendto(sockfd, packet, PAGE_SIZE + sizeof(int), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    memset(ACK, 0, sizeof(ACK));
    pthread_t sender[NUM_THREADS_SENDER];
    pthread_t receiver[NUM_THREADS_RECEIVER];
    pthread_t RTT_Measure[NUM_THREADS_RTT];
    ThreadData thread_data[NUM_THREADS_SENDER];
    long chunk_size_per_thread;
    RTT = -1.0;

    if (argc < 2)
    {
        fprintf(stderr, "missing parameter!\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    chunk_size_per_thread = FILE_SIZE / NUM_THREADS_SENDER;

    for (int i = 0; i < NUM_THREADS_RECEIVER; i++)
    {
        pthread_create(&receiver[i], NULL, thread_revfile, &i);
    }

    for (int i = 0; i < NUM_THREADS_RTT; i++)
    {
        pthread_create(&RTT_Measure[i], NULL, thread_RTT, &i);
    }

    for (int i = 0; i < NUM_THREADS_RTT; i++)
    {
        pthread_join(RTT_Measure[i], NULL);
    }
    printf("Measured RTT: %f s\n", RTT);
    printTimestamp();
    for (int i = 0; i < NUM_THREADS_SENDER; i++)
    {
        thread_data[i].thread_id = i;
        thread_data[i].offset = i * chunk_size_per_thread;
        thread_data[i].chunk_size = chunk_size_per_thread;

        pthread_create(&sender[i], NULL, thread_sendfile, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS_SENDER; i++)
    {
        pthread_join(sender[i], NULL);
    }
    return 0;
}