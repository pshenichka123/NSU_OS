#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define ERROR -1
#define PORT 5005
#define BUFFER_SIZE 1024
#define NO_FLAGS 0

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == ERROR) {
        perror("socket creation failed");
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
            close(sockfd);
            return EXIT_FAILURE;
        }
        buffer[strcspn(buffer, "\n")] = '\0';

        int send_count = sendto(sockfd, buffer, strlen(buffer), NO_FLAGS, (const struct sockaddr*)&servaddr, sizeof(servaddr));
        if (send_count == ERROR)
        {
            perror("recvfrom error");
            close(sockfd);
            return EXIT_FAILURE;
        }
        int recv_count = recvfrom(sockfd, buffer, BUFFER_SIZE, NO_FLAGS, NULL, NULL);
        if (recv_count == ERROR)
        {
            perror("recvfrom error");
            close(sockfd);
            return EXIT_FAILURE;
        }
        buffer[recv_count] = '\0';
        printf("Server echo: %s\n", buffer);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}
