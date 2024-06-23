#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int server_socket;

// Fungsi untuk melakukan koneksi ke server
void connect_to_server() {
    struct sockaddr_in server_addr;

    // Membuat socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Kesalahan pembuatan socket");
        exit(EXIT_FAILURE);
    }

    // Menentukan alamat server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Mengubah alamat IP dari string ke bentuk binary
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Alamat tidak valid/Alamat tidak didukung");
        exit(EXIT_FAILURE);
    }

    // Melakukan koneksi ke server
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Koneksi gagal");
        exit(EXIT_FAILURE);
    }
}

// Fungsi untuk menangani perintah dari pengguna
void handle_command(char *username) {
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        printf("[%s] ", username);
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; 

        // Mengirim perintah ke server
        if (send(server_socket, command, strlen(command), 0) < 0) {
            perror("Gagal mengirim perintah ke server");
            continue;
        }

        // Keluar dari loop jika perintah adalah "EXIT"
        if (strcmp(command, "EXIT") == 0) {
            break;
        }

        // Menerima respons dari server
        memset(response, 0, sizeof(response));
        if (recv(server_socket, response, BUFFER_SIZE - 1, 0) < 0) {
            perror("Gagal menerima respons dari server");
            break;
        }

        // Menampilkan respons dari server
        printf("%s\n", response);

        // Keluar dari loop jika respons menunjukkan pengguna telah keluar dari aplikasi
        if (strstr(response, "Anda telah keluar dari aplikasi") != NULL) {
            break;
        }
    }

    // Menutup socket setelah selesai
    close(server_socket);
}

// Fungsi utama program
int main(int argc, char *argv[]) {
    // Memeriksa jumlah argumen yang benar
    if (argc < 3) {
        printf("Penggunaan: %s REGISTER/LOGIN username -p password\n", argv[0]);
        return 1;
    }

    // Melakukan koneksi ke server
    connect_to_server();

    char command[BUFFER_SIZE];
    memset(command, 0, sizeof(command));

    char username[50];

    // Menangani perintah REGISTER
    if (strcmp(argv[1], "REGISTER") == 0) {
        // Memeriksa jumlah argumen yang benar untuk REGISTER
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Penggunaan: %s REGISTER username -p password\n", argv[0]);
            return 1;
        }

        // Mendapatkan username dan password dari argumen
        snprintf(username, sizeof(username), "%s", argv[2]);
        char *password = argv[4];

        // Membuat perintah untuk dikirim ke server
        snprintf(command, sizeof(command), "REGISTER %s -p %s", username, password);

    // Menangani perintah LOGIN
    } else if (strcmp(argv[1], "LOGIN") == 0) {
        // Memeriksa jumlah argumen yang benar untuk LOGIN
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Penggunaan: %s LOGIN username -p password\n", argv[0]);
            return 1;
        }

        // Mendapatkan username dan password dari argumen
        snprintf(username, sizeof(username), "%s", argv[2]);
        char *password = argv[4];

        // Membuat perintah untuk dikirim ke server
        snprintf(command, sizeof(command), "LOGIN %s -p %s", username, password);
    } else {
        // Jika perintah tidak valid
        printf("Perintah tidak valid\n");
        close(server_socket);
        return 1;
    }

    // Mengirim perintah LOGIN atau REGISTER ke server
    if (send(server_socket, command, strlen(command), 0) < 0) {
        perror("Tidak dapat mengirim perintah ke server");
        return 1;
    }

    char response[BUFFER_SIZE];
    memset(response, 0, sizeof(response));

    // Menerima respons dari server
    if (recv(server_socket, response, BUFFER_SIZE - 1, 0) < 0) {
        perror("Tidak dapat menerima respons dari server");
        return 1;
    }

    // Menampilkan respons dari server
    printf("%s\n", response);

    // Jika respons menunjukkan berhasil login, panggil fungsi handle_command untuk menangani perintah selanjutnya
    if (strstr(response, "berhasil login") != NULL) {
        handle_command(username);
    }

    // Menutup socket setelah selesai
    close(server_socket);
    return 0;
}
