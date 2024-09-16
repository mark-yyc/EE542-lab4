#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr;
    int ret = 0;
    int c = 0;
    char buff[BUFF_SIZE] = {'\0'};
    socklen_t addr_len;

    if (argc < 2)
    {
        fprintf(stderr, "missing parameter!\n");
        exit(1);
    }

    // create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // set address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    printf("Please enter the message: ");
    bzero(buff, 256);
    fgets(buff, 255, stdin);
    char *message = argv[1];

    // send
    ret = sendto(sockfd, message, strlen(message) + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (-1 == ret)
    {
        perror("sendto");
        exit(errno);
    }

    printf("send %d bytes\n", ret);

    // receive
    ret = recv(sockfd, buff, sizeof(buff), 0);
    if (-1 == ret)
    {
        perror("recvfrom");
        exit(errno);
    }

    printf("received %d bytes\n", ret);
    printf("recevied: %s\n", buff);

    return 0;
}