#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 5005
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP echo server listening on port %d\n", PORT);

        while (1) {
            socklen_t len = sizeof(cliaddr);
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                (struct sockaddr*)&cliaddr, &len);
                buffer[n] = '\0';
            printf("Received: %s\n", buffer);

            sendto(sockfd, buffer, n, 0,
                (const struct sockaddr*)&cliaddr, len);
            printf("Echoed back\n");
        }

    return 0;
}