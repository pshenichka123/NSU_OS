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

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == ERROR) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    memset(&servaddr, 0, sizeof(servaddr));

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
        int n;
        n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
        if (n == ERROR)
        {
            perror("recvfrom error");
            break;
        }
        n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (n == ERROR)
        {
            perror("recvfrom error");
            break;
        }
        buffer[n] = '\0';
        printf("Server echo: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
