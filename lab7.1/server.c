#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 5005
#define BUFFER_SIZE 1024
#define ERROR -1
int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  //
    if (sockfd == ERROR) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    int binding_status = bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    if (binding_status == ERROR) {
        perror("bind failed");
        return EXIT_FAILURE;
    }

    printf("UDP echo server listening on port %d\n", PORT);

    while (1) {
        socklen_t src_addr_len = sizeof(cliaddr);
        int recv_count = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&cliaddr, &src_addr_len);
        if (recv_count == ERROR)
        {
            perror("recvfrom error");
            return 0;
        }
        buffer[recv_count] = '\0';
        printf("Received message: %s\n", buffer);

        int send_count = sendto(sockfd, buffer, recv_count, 0, (const struct sockaddr*)&cliaddr, src_addr_len);
        if (send_count == ERROR)
        {
            perror("recvfrom error");
            return 0;
        }
        printf("Echoed back\n");
    }

    return 0;
}