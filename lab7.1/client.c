#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define ERROR -1
#define PORT 5005
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == ERROR) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    while (1) {
        printf("Enter message: ");

        char* message = fgets(buffer, BUFFER_SIZE, stdin);
        if (message == NULL)
        {
            puts("file ended");
            break;
        }
        buffer[strcspn(buffer, "\n")] = '\0';

        int send_count = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
        if (send_count == ERROR)
        {
            perror("recvfrom error");
            break;
        }
        int recv_count = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (recv_count == ERROR)
        {
            perror("recvfrom error");
            break;
        }
        buffer[recv_count] = '\0';
        printf("Server echo: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
