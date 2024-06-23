#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void connect_to_server() {
    int server_fd;
    struct sockaddr_in serv_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to DiscorIT server.\n");

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));


    if (recv(server_fd, buffer, BUFFER_SIZE - 1, 0) < 0) {
        perror("Failed to receive message from server");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buffer);  


    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(server_fd, buffer, BUFFER_SIZE - 1, 0);
        if (valread < 0) {
            perror("Failed to receive message from server");
            exit(EXIT_FAILURE);
        } else if (valread == 0) {
            printf("Server disconnected.\n");
            break;
        } else {
            printf("%s", buffer);  
        }
    }

    close(server_fd);
}

int main(int argc, char *argv[]) {
    if (argc < 4 || strcmp(argv[1], "-channel") != 0 || strcmp(argv[3], "-room") != 0) {
        printf("Usage: %s -channel channel_name -room room_name\n", argv[0]);
        return 1;
    }

    char *channel_name = argv[2];
    char *room_name = argv[4];
    connect_to_server();

    return 0;
}

