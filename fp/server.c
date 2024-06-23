#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int server_fd;

void connect_to_server() {
    struct sockaddr_in serv_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Kesalahan pembuatan socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Alamat tidak valid/Alamat tidak didukung");
        exit(EXIT_FAILURE);
    }

    if (connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Koneksi gagal");
        exit(EXIT_FAILURE);
    }
}

void handle_command(char *username) {
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        printf("[%s] ", username);
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Menghilangkan karakter newline

        if (send(server_fd, command, strlen(command), 0) < 0) {
            perror("Gagal mengirim perintah ke server");
            continue;
        }

        if (strcmp(command, "EXIT") == 0) {
            break;
        }

        memset(response, 0, sizeof(response));
        if (recv(server_fd, response, BUFFER_SIZE - 1, 0) < 0) {
            perror("Gagal menerima respons dari server");
            break;
        }

        printf("%s\n", response);

        if (strstr(response, "Anda telah keluar dari aplikasi") != NULL) {
            break;
        }
    }

    close(server_fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Penggunaan: %s REGISTER/LOGIN username -p password\n", argv[0]);
        return 1;
    }

    connect_to_server();

    char command[BUFFER_SIZE];
    memset(command, 0, sizeof(command));

    char username[50];

    if (strcmp(argv[1], "REGISTER") == 0) {
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Penggunaan: %s REGISTER username -p password\n", argv[0]);
            return 1;
        }

        snprintf(username, sizeof(username), "%s", argv[2]);
        char *password = argv[4];

        snprintf(command, sizeof(command), "REGISTER %s %s", username, password);

        if (send(server_fd, command, strlen(command), 0) < 0) {
            perror("Gagal mengirim perintah ke server");
            return 1;
        }

        char response[BUFFER_SIZE];
        memset(response, 0, sizeof(response));

        if (recv(server_fd, response, BUFFER_SIZE - 1, 0) < 0) {
            perror("Gagal menerima respons dari server");
            return 1;
        }

        printf("%s\n", response);

    } else if (strcmp(argv[1], "LOGIN") == 0) {
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Penggunaan: %s LOGIN username -p password\n", argv[0]);
            return 1;
        }

        snprintf(username, sizeof(username), "%s", argv[2]);
        char *password = argv[4];

        snprintf(command, sizeof(command), "LOGIN %s %s", username, password);

        if (send(server_fd, command, strlen(command), 0) < 0) {
            perror("Gagal mengirim perintah ke server");
            return 1;
        }

        char response[BUFFER_SIZE];
        memset(response, 0, sizeof(response));

        if (recv(server_fd, response, BUFFER_SIZE - 1, 0) < 0) {
            perror("Gagal menerima respons dari server");
            return 1;
        }

        printf("%s\n", response);

        if (strstr(response, "berhasil login") != NULL) {
            handle_command(username);
        }

        close(server_fd);
        return 0;
    } else {
        printf("Perintah tidak valid\n");
        close(server_fd);
        return 1;
    }

    close(server_fd);
    return 0;
}
