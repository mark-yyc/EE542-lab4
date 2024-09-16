#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFF_SIZE 1024

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int server_sockfd, client_sockfd;
    char ch = '\0';
    int ret = 0;
    int recv_len = 0;
    int send_len = 0;
    char buff[BUFF_SIZE] = {'\0'};

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(struct sockaddr_in);

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    // create UDP socket
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // set address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    // bind sockfd with server_addr
    ret = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (-1 == ret)
    {
        perror("bind");
        exit(1);
    }

    // receive from client and send back
    while (1)
    {
        printf("server waiting\n");

        recv_len = recvfrom(server_sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (recv_len < 0)
        {
            perror("recvfrom");
            exit(errno);
        }

        printf("received: %s\n", buff);
        send_len = sendto(server_sockfd, buff, strlen(buff) + 1, 0, (struct sockaddr *)&client_addr, client_addr_len);
        if (-1 == send_len)
        {
            perror("sendto");
            exit(errno);
        }
    }

    close(server_sockfd);
    return 0;
}