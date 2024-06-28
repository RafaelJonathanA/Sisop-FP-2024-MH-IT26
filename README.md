# Laporan Final Project (REVISI)

# Sisop-FP-2024-MH-IT26
## ***KELOMPOK IT 26***
  | Nama      | NRP         |
  |-----------|-------------|
  | Rafael Jonathan Arnoldus   | 5027231006  |
  | Muhammad Abhinaya Al-Faruqi  | 5027231011  |  
  | Danendra Fidel Khansa  | 5027231063  |

## SOAL 

DiscorIT

Disclaimer
Program server, discorit, dan monitor TIDAK DIPERBOLEHKAN menggunakan perintah system();

Bagaimana Program Diakses
Untuk mengakses DiscorIT, user perlu membuka program client (discorit). discorit hanya bekerja sebagai client yang mengirimkan request user kepada server.

Program server berjalan sebagai server yang menerima semua request dari client dan mengembalikan response kepada client sesuai ketentuan pada soal. Program server berjalan sebagai daemon. 

Untuk hanya menampilkan chat, user perlu membuka program client (monitor). Lebih lengkapnya pada poin monitor.
Program client dan server berinteraksi melalui socket.
Server dapat terhubung dengan lebih dari satu client.
#### Tree
```
DiscorIT/
      - channels.csv
      - users.csv
      - channel1/
               - admin/
                        - auth.csv
                        - user.log
               - room1/
                        - chat.csv
               - room2/
                        - chat.csv
               - room3/
                        - chat.csv
      - channel2/
               - admin/
                        - auth.csv
                        - user.log
               - room1/
                        - chat.csv
               - room2/
                        - chat.csv
               - room3/
                        - chat.csv
```


# Keterangan setiap file
```
DiscorIT
users.csv
id_user	int (mulai dari 1)
name		string
password	string (di encrypt menggunakan bcrypt biar ga tembus)
global_role	string (pilihannya: ROOT / USER)

channels.csv
id_channel	int  (mulai dari 1)
channel	string
key		string (di encrypt menggunakan bcrypt biar ga tembus)
Channels
auth.csv
id_user	int
name		string
role		string (pilihannya: ROOT/ADMIN/USER/BANNED) 

user.log
[dd/mm/yyyy HH:MM:SS] admin buat room1
[dd/mm/yyyy HH:MM:SS] user1 masuk ke channel “say hi”
[dd/mm/yyyy HH:MM:SS] admin memberi role1 kepada user1
[dd/mm/yyyy HH:MM:SS] admin ban user1
Rooms
chat.csv
date		int
id_chat	number  (mulai dari 1)
sender 	string
chat		string
```

Autentikasi
Setiap user harus memiliki username dan password untuk mengakses DiscorIT. Username, password, dan global role disimpan dalam file user.csv.
Jika tidak ada user lain dalam sistem, user pertama yang mendaftar otomatis mendapatkan role "ROOT". Username harus bersifat unique dan password wajib di encrypt menggunakan menggunakan bcrypt.

Register
##### Format
```
./discorit REGISTER username -p password
username berhasil register
./discorit REGISTER same_username -p password
username sudah terdaftar
```

##### Contoh
```
./discorit REGISTER qurbancare -p qurban123
qurbancare berhasil register
./discorit REGISTER qurbancare -p qurban123
qurbancare sudah terdaftar
```
Login
##### Format
```
./discorit LOGIN username -p password
username berhasil login
[username] (bisa memberi input setelah tanda kurung)
```

##### Contoh
```
./discorit LOGIN qurbancare -p qurban123 
qurbancare berhasil login
[qurbancare] 
```
Bagaimana DiscorIT digunakan
Perbedaan Akses antara Root, Admin, dan User
Root: Memiliki akses penuh untuk mengelola semua channel, room, dan user. Root adalah akun yang pertama kali mendaftar.
Admin: Memiliki akses untuk mengelola channel dan room yang mereka buat, serta mengelola user dalam channel mereka.
User: Dapat mengirim pesan chat, melihat channel, dan room. user menjadi admin di channel yang mereka buat.
List Channel dan Room
Setelah Login user dapat melihat daftar channel yang tersedia.
##### Format
```
[user] LIST CHANNEL
channel1 channel2 channel3
```

##### Contoh
```
[qurbancare] LIST CHANNEL
care bancar qurb
```
Setelah Masuk Channel user dapat melihat list room dan pengguna dalam channel tersebut
##### Format
```
[user/channel] LIST ROOM
room1 room2 room3
[user/channel] LIST USER
user1 user2 user3
```

##### Contoh
```
[qurbancare/care] LIST ROOM
urban banru runab
[qurbancare/care] LIST USER
root admin qurbancare
```
Akses Channel dan Room
Akses channel admin dan root
##### Format
```
[user] JOIN channel
[user/channel] 
```

##### Contoh
```
[qurbancare] JOIN care
[qurbancare/care] 
```
Ketika user pertama kali masuk ke channel, mereka memiliki akses terbatas. Jika mereka telah masuk sebelumnya, akses mereka meningkat sesuai dengan admin dan root.
##### Format
```
[user] JOIN channel
Key: key
[user/channel] 
```

##### Contoh
```
[qurbancare] JOIN care
Key: care123
[qurbancare/care] 
```
User dapat masuk ke room setelah bergabung dengan channel.
##### Format
```
[user/channel] JOIN room
[user/channel/room] 
```

##### Contoh
```
[qurbancare/care] JOIN urban
[qurbancare/care/urban] 
```
Fitur Chat
Setiap user dapat mengirim pesan dalam chat. ID pesan chat dimulai dari 1 dan semua pesan disimpan dalam file chat.csv. User dapat melihat pesan-pesan chat yang ada dalam room. Serta user dapat edit dan delete pesan yang sudah dikirim dengan menggunakan ID pesan.

##### Format
```
[user/channel/room] CHAT "text"
[user/channel/room] SEE CHAT
sebelumnya
[dd/mm/yyyy HH:MM:SS][id][username] “text”
sesudahnya
[user/channel/room] EDIT CHAT id “text”
[user/channel/room] DEL CHAT id 
```

##### Contoh
```
[qurbancare/care/urban] CHAT “hallo”
[qurbancare/care/urban] SEE CHAT
sebelumnya
[05/06/2024 23:22:12][3][qurbancare] “hallo”
sesudahnya
[qurbancare/care/urban] EDIT CHAT 3 “hi”
[qurbancare/care/urban] DEL CHAT 3
```
Root
Akun yang pertama kali mendaftar otomatis mendapatkan peran "root".
Root dapat masuk ke channel manapun tanpa key dan create, update, dan delete pada channel dan room, mirip dengan admin [D].
Root memiliki kemampuan khusus untuk mengelola user, seperti: list, edit, dan Remove.
##### Format
```
[user] LIST USER
user1 user2 user3
[user] EDIT WHERE user1 -u user01
user1 berhasil diubah menjadi user01
[user] EDIT WHERE user01 -p secretpass
password user01 berhasil diubah
[user] REMOVE user01
user01 berhasil dihapus
```

##### Contoh
```
[root] LIST USER
naupan qurbancare bashmi
[root] EDIT WHERE naupan -u zika
naupan berhasil diubah menjadi zika
[root] EDIT WHERE zika -p 123zika
password zika berhasil diubah
[root] REMOVE zika
zika berhasil dihapus
```
Admin Channel
Setiap user yang membuat channel otomatis menjadi admin di channel tersebut. Informasi tentang user disimpan dalam file auth.csv.
Admin dapat create, update, dan delete pada channel dan room, serta dapat remove, ban, dan unban user di channel mereka.
Channel
Informasi tentang semua channel disimpan dalam file channel.csv. Semua perubahan dan aktivitas user pada channel dicatat dalam file users.log.
##### Format
```
[user] CREATE CHANNEL channel -k key
Channel channel dibuat
[user] EDIT CHANNEL old_channel TO new_channel
old_channel berhasil diubah menjadi new_channel
[user] DEL CHANNEL channel
channel berhasil dihapus
```

##### Contoh
```
[qurbancare] CREATE CHANNEL care -k care123
Channel care dibuat
[qurbancare] EDIT CHANNEL care TO cera
care berhasil diubah menjadi cera
[qurbancare] DEL CHANNEL cera
cera berhasil dihapus
```
Room
Semua perubahan dan aktivitas user pada room dicatat dalam file users.log.
##### Format
```
[user/channel] CREATE ROOM room 
Room room dibuat
[user/channel] EDIT ROOM old_room TO new_room
old_room berhasil diubah menjadi new_room
[user/channel] DEL ROOM room
room berhasil dihapus
[user/channel] DEL ROOM ALL
Semua room dihapus
```
##### Contoh
```
[qurbancare/care] CREATE ROOM urban
Room urban dibuat
[qurbancare/care] EDIT ROOM urban TO nabru
urban berhasil diubah menjadi nabru
[qurbancare/care] DEL ROOM nabru
nabru berhasil dihapus
[qurbancare/care] DEL ROOM ALL
Semua room dihapus
```
Ban 
Admin dapat melakukan ban pada user yang nakal. Aktivitas ban tercatat pada users.log. Ketika di ban, role "user" berubah menjadi "banned". Data tetap tersimpan dan user tidak dapat masuk ke dalam channel.
##### Format
```
[user/channel] BAN user1 
user1 diban
```

##### Contoh
```
[qurbancare/care] BAN pen
pen diban
```
Unban 
Admin dapat melakukan unban pada user yang sudah berperilaku baik. Aktivitas unban tercatat pada users.log. Ketika di unban, role "banned" berubah kembali menjadi "user" dan dapat masuk ke dalam channel.
##### Format
```
[user/channel] UNBAN user1 
user1 kembali
```

##### Contoh
```
[qurbancare/care] UNBAN pen
pen kembali
```
Remove user
Admin dapat remove user dan tercatat pada users.log.
##### Format
```
[user/channel] REMOVE USER user1 
user1 dikick
```

##### Contoh
```
[qurbancare/care] REMOVE USER pen
pen dikick
```

User
User dapat mengubah informasi profil mereka, user yang di ban tidak dapat masuk kedalam channel dan dapat keluar dari room, channel, atau keluar sepenuhnya dari DiscorIT.
Edit User Username
##### Format
```
[user] EDIT PROFILE SELF -u new_username
Profil diupdate
[new_username]
```

##### Contoh
```
[qurbancare] EDIT PROFILE SELF -u pen
Profil diupdate
[pen]
```

	NB: ketika username diubah maka username pada auth.csv ikut berubah. Untuk log dan chat username tidak berubah karena biarkan yang lalu tetap berlalu :)

Edit User Password
##### Format
```
[user] EDIT PROFILE SELF -p new_password
Password diupdate
```

##### Contoh
```
[qurbancare] EDIT PROFILE SELF -p pen123
Password diupdate
```
Banned user
##### Format
```
[user] JOIN channel
anda telah diban, silahkan menghubungi admin
```

##### Contoh
```
[qurbancare] JOIN care
anda telah diban, silahkan menghubungi admin
```
Exit
##### Format
```
[user/channel/room] EXIT
[user/channel] EXIT
[user] EXIT
```

##### Contoh
```
[qurbancare/care/urban] EXIT
[qurbancare/care] EXIT
[qurbancare] EXIT
```
Error Handling
Jika ada command yang tidak sesuai penggunaannya. Maka akan mengeluarkan pesan error dan tanpa keluar dari program client.
Monitor
User dapat menampilkan isi chat secara real-time menggunakan program monitor. Jika ada perubahan pada isi chat, perubahan tersebut akan langsung ditampilkan di terminal.
Sebelum dapat menggunakan monitor, pengguna harus login terlebih dahulu dengan cara yang mirip seperti login di DiscorIT.
Untuk keluar dari room dan menghentikan program monitor dengan perintah "EXIT".
Monitor dapat digunakan untuk menampilkan semua chat pada room, mulai dari chat pertama hingga chat yang akan datang nantinya.


##### Format
```
[username] -channel channel_name -room room_name
~isi chat~
sebelumnya
[dd/mm/yyyy HH:MM:SS][id][username] “text”
sesudahnya
[user/channel/room] EXIT
[user] EXIT
```

##### Contoh
```
[qurbancare] -channel care -room urban
~isi chat~
sebelumnya
[05/06/2024 23:22:12][3][qurbancare] “hallo”
sesudahnya
[qurbancare/care/urban] EXIT
[qurbancare] EXIT
```

Changelog
```
[13/06/2024] menambahkan penjelasan pada bagian Bagaimana Program Diakses
```


## Pekerjaan (Sebelum Revisi)

### discorit.c 
```c
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
```
### server.c 
```c
//server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <crypt.h>
#include <time.h>
#include <dirent.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5
#define CSV_FILE "/home/ubuntu/FP/users.csv"
#define BASE_DIR "/home/ubuntu/FP"
#define CHANNELS_FILE "/home/ubuntu/FP/channels.csv"
#define LOG_FILE "/home/ubuntu/FP/users.log"
#define AUTH_FILE "auth.csv"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int clients[MAX_CLIENTS];

typedef struct {
    int id_user;
    char name[50];
    char password[100];
    char global_role[10];
} User;

// Function declarations for handling client requests
void *handle_client(void *arg);
void register_user(int client_socket, char *username, char *password);
void login_user(int client_socket, char *username, char *password);
void list_channels(int client_socket);
void list_rooms(int client_socket, char *channel);
void list_users(int client_socket, char *channel);
void list_users_root(int client_socket);
void join_channel(int client_socket, char *username, char *channel, char *key);
void join_room(int client_socket, char *username, char *channel, char *room);
void create_channel(int client_socket, char *username, char *channel, char *key);
void edit_channel(int client_socket, char *old_channel, char *new_channel);
void delete_channel(int client_socket, char *channel);
void create_room(int client_socket, char *channel, char *room);
void edit_room(int client_socket, char *channel, char *old_room, char *new_room);
void delete_room(int client_socket, char *channel, char *room);
void delete_all_rooms(int client_socket, char *channel);
void log_user_activity(const char *username, const char *activity);
void edit_user_username(int client_socket, char *old_username, char *new_username);
void edit_user_password(int client_socket, char *username, char *new_password);
void remove_user(int client_socket, char *username);
void edit_user_username_self(int client_socket, char *username, char *new_username);
void edit_user_password_self(int client_socket, char *username, char *new_password);

// Error handling function
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Function to daemonize the server
void daemonize() {
    pid_t pid, sid;

    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Terminate parent process
    }

    // On success: The child process becomes session leader
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // Change the current working directory to root
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;

    // Daemonize the server
    daemonize();

    // Now the process is daemonized

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) error("ERROR opening socket");

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
        error("ERROR on binding");

    listen(server_socket, 5);

    printf("Server started on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) error("ERROR on accept");

        pthread_create(&tid, NULL, handle_client, (void *)&client_socket);
    }

    close(server_socket);
    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    char command[50], username[50], password[50], channel[50], key[50], room[50], new_name[50];
    char logged_in_username[50] = "";
    char logged_in_global_role[10] = "";

    bzero(buffer, BUFFER_SIZE);

    while (read(client_socket, buffer, BUFFER_SIZE - 1) > 0) {
        if (strncmp(buffer, "REGISTER ", 9) == 0) {
            sscanf(buffer + 9, "%s -p %s", username, password);
            register_user(client_socket, username, password);
        } else if (strncmp(buffer, "LOGIN ", 6) == 0) {
            sscanf(buffer + 6, "%s -p %s", username, password);
            login_user(client_socket, username, password);
            // Save logged in username and global_role
            strcpy(logged_in_username, username);
            strcpy(logged_in_global_role, "ROOT"); // Assume ROOT for demonstration purposes
        } else if (strncmp(buffer, "CREATE CHANNEL ", 15) == 0) {
            sscanf(buffer + 15, "%s -k %s", channel, key);
            create_channel(client_socket, logged_in_username, channel, key);
        } else if (strncmp(buffer, "EDIT CHANNEL ", 13) == 0) {
            sscanf(buffer + 13, "%s TO %s", channel, new_name);
            edit_channel(client_socket, channel, new_name);
        } else if (strncmp(buffer, "DEL CHANNEL ", 12) == 0) {
            sscanf(buffer + 12, "%s", channel);
            delete_channel(client_socket, channel);
        } else if (strncmp(buffer, "CREATE ROOM ", 12) == 0) {
            sscanf(buffer + 12, "%s %s", channel, room);
            create_room(client_socket, channel, room);
        } else if (strncmp(buffer, "EDIT ROOM ", 10) == 0) {
            sscanf(buffer + 10, "%s %s TO %s", channel, room, new_name);
            edit_room(client_socket, channel, room, new_name);
        } else if (strncmp(buffer, "DEL ROOM ", 9) == 0) {
            sscanf(buffer + 9, "%s %s", channel, room);
            delete_room(client_socket, channel, room);
        } else if (strncmp(buffer, "DEL ROOM ALL", 12) == 0) {
            sscanf(buffer + 12, "%s", channel);
            delete_all_rooms(client_socket, channel);
        } else if (strncmp(buffer, "LIST CHANNEL", 12) == 0) {
            list_channels(client_socket);
        } else if (strncmp(buffer, "JOIN ", 5) == 0) {
            if (sscanf(buffer + 5, "%s Key: %s", channel, key) == 2) {
                join_channel(client_socket, logged_in_username, channel, key);
            } else if (sscanf(buffer + 5, "%s", channel) == 1) {
                join_channel(client_socket, logged_in_username, channel, NULL);
            } else {
                write(client_socket, "ERROR: Invalid command\n", 23);
            }
        } else if (sscanf(buffer, "JOIN %s %s", channel, room) == 2) {
            join_room(client_socket, logged_in_username, channel, room);
        } else if (strncmp(buffer, "LIST ROOM ", 10) == 0) {
            sscanf(buffer + 10, "%s", channel);
            list_rooms(client_socket, channel);
        } else if (strncmp(buffer, "LIST USER", 9) == 0) {
            if (strcmp(logged_in_global_role, "ROOT") == 0) {
                list_users_root(client_socket);
            } else {
                write(client_socket, "ERROR: Permission denied\n", 25);
            }
        } else if (strncmp(buffer, "EDIT PROFILE SELF -u ", 20) == 0) {
            sscanf(buffer + 20, "%s", new_name);
            edit_user_username_self(client_socket, logged_in_username, new_name);
        } else if (strncmp(buffer, "EDIT PROFILE SELF -p ", 20) == 0) {
            sscanf(buffer + 20, "%s", password);
            edit_user_password_self(client_socket, logged_in_username, password);
        } else if (strncmp(buffer, "EDIT WHERE ", 11) == 0) {
            if (strcmp(logged_in_global_role, "ROOT") == 0) {
                if (strstr(buffer, " -u ")) {
                    sscanf(buffer + 11, "%s -u %s", username, new_name);
                    edit_user_username(client_socket, username, new_name);
                } else if (strstr(buffer, " -p ")) {
                    sscanf(buffer + 11, "%s -p %s", username, password);
                    edit_user_password(client_socket, username, password);
                } else {
                    write(client_socket, "ERROR: Invalid command\n", 23);
                }
            } else {
                write(client_socket, "ERROR: Permission denied\n", 25);
            }
        } else if (strncmp(buffer, "REMOVE ", 7) == 0) {
            if (strcmp(logged_in_global_role, "ROOT") == 0) {
                sscanf(buffer + 7, "%s", username);
                remove_user(client_socket, username);
            } else {
                write(client_socket, "ERROR: Permission denied\n", 25);
            }
        } else if (strncmp(buffer, "EXIT", 4) == 0) {
            break;
        } else {
            write(client_socket, "ERROR: Invalid command\n", 23);
        }
        bzero(buffer, BUFFER_SIZE);
    }

    close(client_socket);
    return NULL;
}


void register_user(int client_socket, char *username, char *password) {
    FILE *fp;
    char buffer[BUFFER_SIZE];
    User user;

    fp = fopen(CSV_FILE, "a+");
    if (fp == NULL) error("ERROR opening file");

    while (fscanf(fp, "%d,%49[^,],%99[^,],%9[^\n]\n", &user.id_user, user.name, user.password, user.global_role) != EOF) {
        if (strcmp(user.name, username) == 0) {
            snprintf(buffer, BUFFER_SIZE, "%s sudah terdaftar\n", username);
            write(client_socket, buffer, strlen(buffer));
            fclose(fp);
            return;
        }
    }

    int id_user = ftell(fp) == 0 ? 1 : user.id_user + 1;
    char *encrypted_password = crypt(password, "$6$");

    fprintf(fp, "%d,%s,%s,%s\n", id_user, username, encrypted_password, id_user == 1 ? "ROOT" : "USER");

    snprintf(buffer, BUFFER_SIZE, "%s berhasil register\n", username);
    write(client_socket, buffer, strlen(buffer));

    fclose(fp);
}


void login_user(int client_socket, char *username, char *password) {
    FILE *fp;
    char buffer[BUFFER_SIZE];
    User user;
    char *encrypted_password = crypt(password, "$6$");

    fp = fopen(CSV_FILE, "r");
    if (fp == NULL) error("ERROR opening file");

    while (fscanf(fp, "%d,%49[^,],%99[^,],%9[^\n]\n", &user.id_user, user.name, user.password, user.global_role) != EOF) {
        if (strcmp(user.name, username) == 0 && strcmp(user.password, encrypted_password) == 0) {
            snprintf(buffer, BUFFER_SIZE, "%s berhasil login\n", username);
            write(client_socket, buffer, strlen(buffer));
            log_user_activity(username, "login");
            fclose(fp);
            return;
        }
    }

    write(client_socket, "ERROR: Invalid username or password\n", 36);
    fclose(fp);
}

void create_channel(int client_socket, char *username, char *channel, char *key) {
    FILE *fp;
    char buffer[BUFFER_SIZE];

    // Add channel to channels.csv
    pthread_mutex_lock(&lock);
    fp = fopen(CHANNELS_FILE, "a+");
    if (fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening channels file");
    }

    fprintf(fp, "%s,%s\n", channel, key);
    fclose(fp);
    pthread_mutex_unlock(&lock);

    // Create directory structure
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/%s", BASE_DIR, channel);
    mkdir(path, 0755);

    snprintf(buffer, BUFFER_SIZE, "Channel %s dibuat\n", channel);
    write(client_socket, buffer, strlen(buffer));

    // Log user activity
    char log_msg[BUFFER_SIZE];
    snprintf(log_msg, BUFFER_SIZE, "[%s] CREATE CHANNEL %s -k %s\n", username, channel, key);
    log_user_activity(username, log_msg);
}

void edit_channel(int client_socket, char *old_channel, char *new_channel) {
    FILE *fp, *temp_fp;
    char buffer[BUFFER_SIZE], channel[50], key[50];
    int found = 0;

    pthread_mutex_lock(&lock);
    fp = fopen(CHANNELS_FILE, "r");
    temp_fp = fopen("/tmp/temp_channels.csv", "w");
    if (fp == NULL || temp_fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening channels file");
    }

    while (fscanf(fp, "%49[^,],%49[^\n]\n", channel, key) != EOF) {
        if (strcmp(channel, old_channel) == 0) {
            fprintf(temp_fp, "%s,%s\n", new_channel, key);
            found = 1;
        } else {
            fprintf(temp_fp, "%s,%s\n", channel, key);
        }
    }

    fclose(fp);
    fclose(temp_fp);
    remove(CHANNELS_FILE);
    rename("/tmp/temp_channels.csv", CHANNELS_FILE);
    pthread_mutex_unlock(&lock);

if (found) {
        snprintf(buffer, BUFFER_SIZE, "Channel %s berhasil diubah menjadi %s\n", old_channel, new_channel);
        write(client_socket, buffer, strlen(buffer));

        // Log user activity
        char log_msg[BUFFER_SIZE];
        snprintf(log_msg, BUFFER_SIZE, "[%s] EDIT CHANNEL TO %s\n", old_channel, new_channel);
        log_user_activity(old_channel, log_msg);

        // Rename directory
        char old_path[BUFFER_SIZE], new_path[BUFFER_SIZE];
        snprintf(old_path, BUFFER_SIZE, "%s/%s", BASE_DIR, old_channel);
        snprintf(new_path, BUFFER_SIZE, "%s/%s", BASE_DIR, new_channel);
        rename(old_path, new_path);
    } else {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Channel %s tidak ditemukan\n", old_channel);
        write(client_socket, buffer, strlen(buffer));
    }
}    
void delete_channel(int client_socket, char *channel) {
    FILE *fp, *temp_fp;
    char buffer[BUFFER_SIZE], temp_channel[50], key[50];
    int found = 0;

    pthread_mutex_lock(&lock);
    fp = fopen(CHANNELS_FILE, "r");
    temp_fp = fopen("/tmp/temp_channels.csv", "w");
    if (fp == NULL || temp_fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening channels file");
    }

    while (fscanf(fp, "%49[^,],%49[^\n]\n", temp_channel, key) != EOF) {
        if (strcmp(temp_channel, channel) == 0) {
            found = 1;
        } else {
            fprintf(temp_fp, "%s,%s\n", temp_channel, key);
        }
    }

    fclose(fp);
    fclose(temp_fp);
    remove(CHANNELS_FILE);
    rename("/tmp/temp_channels.csv", CHANNELS_FILE);
    pthread_mutex_unlock(&lock);

    if (found) {
        snprintf(buffer, BUFFER_SIZE, "Channel %s berhasil dihapus\n", channel);
        write(client_socket, buffer, strlen(buffer));

        // Log user activity
        char log_msg[BUFFER_SIZE];
        snprintf(log_msg, BUFFER_SIZE, "[%s] DEL CHANNEL %s\n", channel, channel);
        log_user_activity(channel, log_msg);

        // Remove directory
        char path[BUFFER_SIZE];
        snprintf(path, BUFFER_SIZE, "%s/%s", BASE_DIR, channel);
        rmdir(path);
    } else {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Channel %s tidak ditemukan\n", channel);
        write(client_socket, buffer, strlen(buffer));
    }
}

void create_room(int client_socket, char *channel, char *room) {
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/%s/%s", BASE_DIR, channel, room);
    mkdir(path, 0755);

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Room %s dibuat\n", room);
    write(client_socket, buffer, strlen(buffer));

    // Log user activity
    char log_msg[BUFFER_SIZE];
    snprintf(log_msg, BUFFER_SIZE, "[%s/%s] CREATE ROOM %s\n", channel, room, room);
    log_user_activity(channel, log_msg);
}

void edit_room(int client_socket, char *channel, char *old_room, char *new_room) {
    char old_path[BUFFER_SIZE], new_path[BUFFER_SIZE];
    snprintf(old_path, BUFFER_SIZE, "%s/%s/%s", BASE_DIR, channel, old_room);
    snprintf(new_path, BUFFER_SIZE, "%s/%s/%s", BASE_DIR, channel, new_room);
    rename(old_path, new_path);

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Room %s berhasil diubah menjadi %s\n", old_room, new_room);
    write(client_socket, buffer, strlen(buffer));

    // Log user activity
    char log_msg[BUFFER_SIZE];
    snprintf(log_msg, BUFFER_SIZE, "[%s/%s] EDIT ROOM %s TO %s\n", channel, old_room, old_room, new_room);
    log_user_activity(channel, log_msg);
}

void delete_room(int client_socket, char *channel, char *room) {
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/%s/%s", BASE_DIR, channel, room);
    rmdir(path);

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Room %s berhasil dihapus\n", room);
    write(client_socket, buffer, strlen(buffer));

    // Log user activity
    char log_msg[BUFFER_SIZE];
    snprintf(log_msg, BUFFER_SIZE, "[%s/%s] DEL ROOM %s\n", channel, room, room);
    log_user_activity(channel, log_msg);
}

void delete_all_rooms(int client_socket, char *channel) {
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/%s", BASE_DIR, channel);

    DIR *dir = opendir(path);
    struct dirent *entry;
    if (dir == NULL) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "ERROR: Channel %s tidak ditemukan\n", channel);
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            snprintf(path, BUFFER_SIZE, "%s/%s/%s", BASE_DIR, channel, entry->d_name);
            rmdir(path);
        }
    }

    closedir(dir);

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Semua room di channel %s berhasil dihapus\n", channel);
    write(client_socket, buffer, strlen(buffer));

    // Log user activity
    char log_msg[BUFFER_SIZE];
    snprintf(log_msg, BUFFER_SIZE, "[%s] DEL ROOM ALL\n", channel);
    log_user_activity(channel, log_msg);
}

void log_user_activity(const char *username, const char *activity) {
    FILE *fp;
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0'; // Remove newline character

    pthread_mutex_lock(&lock);
    fp = fopen(LOG_FILE, "a");
    if (fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening log file");
    }

    fprintf(fp, "[%s] %s %s\n", timestamp, username, activity);
    fclose(fp);
    pthread_mutex_unlock(&lock);
}

void list_channels(int client_socket) {
    FILE *fp;
    char buffer[BUFFER_SIZE], channel[50], key[50];

    pthread_mutex_lock(&lock);
    fp = fopen(CHANNELS_FILE, "r");
    if (fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening channels file");
    }

    // Check if the file is empty
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    if (file_size == 0) {
        snprintf(buffer, BUFFER_SIZE, "Tidak ada channel yang tersedia\n");
        write(client_socket, buffer, strlen(buffer));
        fclose(fp);
        pthread_mutex_unlock(&lock);
        return;
    }

    // File is not empty, reset to beginning and read channels
    fseek(fp, 0, SEEK_SET);
    bzero(buffer, BUFFER_SIZE);
    while (fscanf(fp, "%49[^,],%49[^\n]\n", channel, key) == 2) {
        strcat(buffer, channel);
        strcat(buffer, "\n");
    }
    fclose(fp);
    pthread_mutex_unlock(&lock);

    write(client_socket, buffer, strlen(buffer));
}


void list_rooms(int client_socket, char *channel) {
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/%s", BASE_DIR, channel);

    DIR *dir = opendir(path);
    struct dirent *entry;
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    if (dir == NULL) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Channel %s tidak ditemukan\n", channel);
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            strcat(buffer, entry->d_name);
            strcat(buffer, "\n");
        }
    }

    closedir(dir);

    write(client_socket, buffer, strlen(buffer));
}

void list_users(int client_socket, char *channel) {
    FILE *fp;
    char path[BUFFER_SIZE], buffer[BUFFER_SIZE], user[50], password[100], role[10];
    int id;

    snprintf(path, BUFFER_SIZE, "%s/%s/%s", BASE_DIR, channel, AUTH_FILE);
    fp = fopen(path, "r");
    if (fp == NULL) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Cannot open auth file for channel %s\n", channel);
        write(client_socket, buffer, strlen(buffer));
        return;
    }
}

void join_channel(int client_socket, char *username, char *channel, char *key) {
    FILE *fp;
    char path[BUFFER_SIZE], buffer[BUFFER_SIZE], stored_channel[50], stored_key[50];

    pthread_mutex_lock(&lock);
    fp = fopen(CHANNELS_FILE, "r");
    if (fp == NULL) {
        pthread_mutex_unlock(&lock);
        snprintf(buffer, BUFFER_SIZE, "ERROR: Cannot open channels file\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    int found = 0;
    while (fscanf(fp, "%49[^,],%49[^\n]\n", stored_channel, stored_key) == 2) {
        if (strcmp(stored_channel, channel) == 0) {
            if (key == NULL || (stored_key != NULL && strcmp(stored_key, key) == 0)) {
                found = 1;
                break;
            }
        }
    }
    fclose(fp);
    pthread_mutex_unlock(&lock);

    if (!found) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Channel not found or incorrect key\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    // Create or open the auth.csv file in the channel directory
    snprintf(path, BUFFER_SIZE, "%s/%s", BASE_DIR, channel);
    if (mkdir(path, 0777) != 0 && errno != EEXIST) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Cannot create or access channel directory\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    snprintf(path, BUFFER_SIZE, "%s/%s/auth.csv", BASE_DIR, channel);
    fp = fopen(path, "a+");
    if (fp == NULL) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Cannot open auth file\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    if (file_size == 0) {
        fprintf(fp, "id_user,name,password,global_role\n");
    }
    fseek(fp, 0, SEEK_SET);

    int id = 1;
    char stored_name[50], stored_password[100], stored_role[10];
    while (fscanf(fp, "%d,%49[^,],%99[^,],%9[^\n]\n", &id, stored_name, stored_password, stored_role) == 4) {
        if (strcmp(stored_name, username) == 0) {
            fclose(fp);

            // Send the prompt for user/channel interaction
            snprintf(buffer, BUFFER_SIZE, "[%s/%s] ", username, channel);
            write(client_socket, buffer, strlen(buffer));
            return;
        }
        id++;
    }

    // If user not found, add the user to auth.csv
    fprintf(fp, "%d,%s,%s,-\n", id, username, "-");
    fclose(fp);

    // Send the prompt for user/channel interaction
    snprintf(buffer, BUFFER_SIZE, "[%s/%s] ", username, channel);
    write(client_socket, buffer, strlen(buffer));
}

void join_room(int client_socket, char *username, char *channel, char *room) {
    char path[BUFFER_SIZE], buffer[BUFFER_SIZE];

    snprintf(path, BUFFER_SIZE, "%s/%s/%s", BASE_DIR, channel, room);
    if (mkdir(path, 0777) != 0 && errno != EEXIST) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Cannot create or access room directory\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    snprintf(buffer, BUFFER_SIZE, "You have joined room %s in channel %s\n", room, channel);
    write(client_socket, buffer, strlen(buffer));

}

void list_users_root(int client_socket) {
    FILE *fp;
    char buffer[BUFFER_SIZE], username[50];

    pthread_mutex_lock(&lock);
    fp = fopen(CSV_FILE, "r");
    if (fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening users file");
    }

    bzero(buffer, BUFFER_SIZE);
    while (fscanf(fp, "%*d,%[^,],%*s,%*s\n", username) == 1) {
        strcat(buffer, username);
        strcat(buffer, " ");
    }
    fclose(fp);
    pthread_mutex_unlock(&lock);

    write(client_socket, buffer, strlen(buffer));
}

void edit_user_username(int client_socket, char *old_username, char *new_username) {
    FILE *fp, *temp_fp;
    char temp_file[1024], username[50], password[100], global_role[10];
    int id;
    int found = 0;

    pthread_mutex_lock(&lock);
    fp = fopen(CSV_FILE, "r");
    temp_fp = fopen("temp.csv", "w");
    if (fp == NULL || temp_fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening users file");
    }

    while (fscanf(fp, "%d,%[^,],%[^,],%s\n", &id, username, password, global_role) == 4) {
        if (strcmp(username, old_username) == 0) {
            fprintf(temp_fp, "%d,%s,%s,%s\n", id, new_username, password, global_role);
            found = 1;
        } else {
            fprintf(temp_fp, "%d,%s,%s,%s\n", id, username, password, global_role);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (!found) {
        remove("temp.csv");
        pthread_mutex_unlock(&lock);
        write(client_socket, "ERROR: User not found\n", 22);
        return;
    }

    remove(CSV_FILE);
    rename("temp.csv", CSV_FILE);

    pthread_mutex_unlock(&lock);

    char success_msg[100];
    snprintf(success_msg, sizeof(success_msg), "username %s berhasil diubah menjadi %s\n", old_username, new_username);
    write(client_socket, success_msg, strlen(success_msg));
}

void edit_user_password(int client_socket, char *username, char *new_password) {
    FILE *fp, *temp_fp;
    char salt[20], *encrypted_password;
    char name[50], password[100], global_role[10];
    int id;
    int found = 0;

    // Create salt for crypt
    snprintf(salt, sizeof(salt), "$6$%ld$", time(NULL));

    // Encrypt the new password
    encrypted_password = crypt(new_password, salt);

    pthread_mutex_lock(&lock);
    fp = fopen(CSV_FILE, "r");
    temp_fp = fopen("temp.csv", "w");
    if (fp == NULL || temp_fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening users file");
    }

    while (fscanf(fp, "%d,%[^,],%[^,],%s\n", &id, name, password, global_role) == 4) {
        if (strcmp(name, username) == 0) {
            fprintf(temp_fp, "%d,%s,%s,%s\n", id, name, encrypted_password, global_role);
            found = 1;
        } else {
            fprintf(temp_fp, "%d,%s,%s,%s\n", id, name, password, global_role);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (!found) {
        remove("temp.csv");
        pthread_mutex_unlock(&lock);
        write(client_socket, "ERROR: User not found\n", 22);
        return;
    }

    remove(CSV_FILE);
    rename("temp.csv", CSV_FILE);

    pthread_mutex_unlock(&lock);

    char success_msg[100];
    snprintf(success_msg, sizeof(success_msg), "password %s berhasil diubah\n", username);
    write(client_socket, success_msg, strlen(success_msg));
}

void remove_user(int client_socket, char *username) {
    FILE *fp, *temp_fp;
    char temp_file[1024], name[50], password[100], global_role[10];
    int id;
    int found = 0;

    pthread_mutex_lock(&lock);
    fp = fopen(CSV_FILE, "r");
    temp_fp = fopen("temp.csv", "w");
    if (fp == NULL || temp_fp == NULL) {
        pthread_mutex_unlock(&lock);
        error("ERROR opening users file");
    }

    while (fscanf(fp, "%d,%[^,],%[^,],%s\n", &id, name, password, global_role) == 4) {
        if (strcmp(name, username) == 0) {
            found = 1;
        } else {
            fprintf(temp_fp, "%d,%s,%s,%s\n", id, name, password, global_role);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (!found) {
        remove("temp.csv");
        pthread_mutex_unlock(&lock);
        write(client_socket, "ERROR: User not found\n", 22);
        return;
    }

    remove(CSV_FILE);
    rename("temp.csv", CSV_FILE);

    pthread_mutex_unlock(&lock);

    char success_msg[100];
    snprintf(success_msg, sizeof(success_msg), "%s berhasil dihapus\n", username);
    write(client_socket, success_msg, strlen(success_msg));
}

void edit_user_username_self(int client_socket, char *username, char *new_username) {
    FILE *fp1, *fp2;
    char temp[] = "/home/ubuntu/FP/temp.csv";
    char line[BUFFER_SIZE];
    int found = 0;

    pthread_mutex_lock(&lock);
    fp1 = fopen(CSV_FILE, "r");
    if (fp1 == NULL) {
        perror("Error opening file");
        pthread_mutex_unlock(&lock);
        return;
    }
    fp2 = fopen(temp, "w");
    if (fp2 == NULL) {
        perror("Error opening file");
        fclose(fp1);
        pthread_mutex_unlock(&lock);
        return;
    }

    // Read and edit username in users.csv
    while (fgets(line, BUFFER_SIZE, fp1) != NULL) {
        char *token = strtok(line, ",");
        int id_user = atoi(token);
        char name[50];
        strcpy(name, strtok(NULL, ","));
        char password[100];
        strcpy(password, strtok(NULL, ","));
        char global_role[10];
        strcpy(global_role, strtok(NULL, "\n"));

        if (strcmp(name, username) == 0) {
            fprintf(fp2, "%d,%s,%s,%s\n", id_user, new_username, password, global_role);
            found = 1;
        } else {
            fprintf(fp2, "%d,%s,%s,%s\n", id_user, name, password, global_role);
        }
    }

    fclose(fp1);
    fclose(fp2);

    // Rename temp.csv to users.csv
    remove(CSV_FILE);
    rename(temp, CSV_FILE);

    pthread_mutex_unlock(&lock);

    if (found) {
        char msg[] = "Profil diupdate\n";
        write(client_socket, msg, strlen(msg));
    } else {
        char msg[] = "ERROR: Username not found\n";
        write(client_socket, msg, strlen(msg));
    }
}

void edit_user_password_self(int client_socket, char *username, char *new_password) {
    FILE *fp1, *fp2;
    char temp[] = "/home/ubuntu/FP/temp.csv";
    char line[BUFFER_SIZE];
    int found = 0;

    pthread_mutex_lock(&lock);
    fp1 = fopen(CSV_FILE, "r");
    if (fp1 == NULL) {
        perror("Error opening file");
        pthread_mutex_unlock(&lock);
        return;
    }
    fp2 = fopen(temp, "w");
    if (fp2 == NULL) {
        perror("Error opening file");
        fclose(fp1);
        pthread_mutex_unlock(&lock);
        return;
    }

    // Read and edit password in users.csv
    while (fgets(line, BUFFER_SIZE, fp1) != NULL) {
        char *token = strtok(line, ",");
        int id_user = atoi(token);
        char name[50];
        strcpy(name, strtok(NULL, ","));
        char password[100];
        strcpy(password, strtok(NULL, ","));
        char global_role[10];
        strcpy(global_role, strtok(NULL, "\n"));

        if (strcmp(name, username) == 0) {
            fprintf(fp2, "%d,%s,%s,%s\n", id_user, name, new_password, global_role);
            found = 1;
        } else {
            fprintf(fp2, "%d,%s,%s,%s\n", id_user, name, password, global_role);
        }
    }

    fclose(fp1);
    fclose(fp2);

    // Rename temp.csv to users.csv
    remove(CSV_FILE);
    rename(temp, CSV_FILE);

    pthread_mutex_unlock(&lock);

    if (found) {
        char msg[] = "Password diupdate\n";
        write(client_socket, msg, strlen(msg));
    } else {
        char msg[] = "ERROR: Username not found\n";
        write(client_socket, msg, strlen(msg));
    }
}
```
### monitor.c
```c
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
```

#### Penjelasan Kode (Sebelum Revisi)
**discorit.c**  

Kode ini bekerja sebagai penghubung antara inputan pengguna dengan server yang bekerja sebagai pemproses inputan tersebut sehingga dapat menjalankan sesuai dengan perintah yang diberikan bisa dibilang discorit ini seperti front-end nya sedangkan server back-end yang memprosesnya dimana memiliki 3 bagain utama yaitu: 

Fungsi Utama (main)
Fungsi main menangani tugas-tugas berikut:

- Memvalidasi argumen baris perintah.
- Menghubungkan ke server.
- Membangun dan mengirim perintah REGISTER atau LOGIN ke server.
- Menerima dan mencetak respons dari server.
- Jika login berhasil, memanggil handle_command untuk menangani perintah pengguna selanjutnya.

Untuk menghubungkan ke Server dengan Fungsi connect_to_server:

- Membuat soket.
- Menyiapkan alamat server.
- Menghubungkan ke server di 127.0.0.1:12345.

Untuk Penanganan Perintah dengan Fungsi handle_command:

- Membaca perintah dari input standar.
- Mengirim perintah ke server.
- Menerima dan mencetak respons dari server.
- Melanjutkan menangani perintah sampai pengguna mengetik EXIT atau server mengindikasikan pengguna telah keluar.

Dan memiliki beberapa penanganan kesalahan atau pesan error yang muncul saat terjadi sesuatu seperti saat tidak bisa connect ke server dan lainnya. 

**server.c** 
Server aplikasi ini menangani banyak klien dan menyediakan berbagai fungsionalitas seperti registrasi pengguna, login, manajemen saluran dan ruang, serta pencatatan aktivitas pengguna. Server ini dirancang untuk berjalan sebagai daemon, memberikan layanan latar belakang yang berkelanjutan sehingga tidak perlu setiap kali dibutuhkan dihidupkan.

Seperti yang dijelaskan sebelumnya pada bagian discorit bahwa server ini bekerja seperti back-end dengan menunggu inputan pada front-end yaitu discorit dan dalam server terdapat beberapa Fitur yaitu : 

- Manajemen Pengguna

    - Registrasi pengguna baru
    - Login pengguna
    - Edit profil pengguna (username dan password)
    - Hapus pengguna (Khusus ROOT)

- Buat, edit, dan hapus CHANNEL

    - Gabung CHANNEL dengan kunci yang sesuai dan orang yang pertama masuk atau ROOT bisa masuk tanpa kunci 

- Buat, edit, dan hapus ROOM dalam CHANNEL

    - Gabung ROOM  
    - Pencatatan Aktivitas ke dalam log yang ada pada bagian folder admin 

- Struktur File
    - Server.c: File sumber kode utama aplikasi server.
    - users.csv: File CSV berisi data pengguna yang terdaftar.
    - channels.csv: File CSV berisi data saluran yang dibuat.
    - users.log: File log berisi catatan aktivitas pengguna.
    - auth.csv: File CSV berisi data autentikasi dalam sebuah channel sehingga apabila saat ingin masuk ke channel namun keynya salah maka tidak dapat masuk 

- Cara Kerja
    - Daemonize Server: Server diatur untuk berjalan sebagai daemon dengan menggunakan fungsi daemonize(), yang melakukan forking proses induk dan menjalankan proses anak sebagai session leader, lalu mengubah direktori kerja ke root dan menutup file deskriptor standar.

    - Menerima Koneksi Klien: Server mendengarkan koneksi masuk pada port yang telah ditentukan (PORT 12345) dan menerima koneksi klien baru, lalu membuat thread baru untuk menangani setiap klien menggunakan fungsi handle_client().

    - Menangani Permintaan Klien: Fungsi handle_client() membaca perintah dari klien dan memanggil fungsi yang sesuai untuk menangani perintah tersebut, seperti registrasi, login, membuat atau mengedit saluran dan ruang, serta mencatat aktivitas pengguna.

    - Manajemen Pengguna: Fungsi seperti register_user() dan login_user() menangani registrasi dan login pengguna dengan memanipulasi data di file CSV.

    - Manajemen Saluran dan Ruang: Fungsi seperti create_channel(), edit_channel(), delete_channel(), create_room(), edit_room(), dan delete_room() mengatur pembuatan, pengeditan, dan penghapusan saluran dan ruang, serta struktur direktori yang sesuai.

    - Pencatatan Aktivitas: Fungsi log_user_activity() mencatat aktivitas pengguna ke dalam file log.

**monitor.c** 

Monitor.c berfungsi untuk melihat chat yang dilakukan oleh client secara real time tanpa harus menuliskan SEE CHAT dan monitor hanya bisa melihat satu room saja dalam satu waktu, dengan cara kerjanya  yaitu pertama terhubung ke server dengan menggunakan fungsi connect_to_server yang dimana bekerja dengan: 

    1. Membuat Socket: Membuat socket menggunakan socket(AF_INET, SOCK_STREAM, 0). Jika gagal, program keluar dengan pesan kesalahan.
    2. Mengatur Alamat Server: Mengatur alamat server (127.0.0.1) dan port (12345) menggunakan inet_pton.
    3. Menghubungkan ke Server: Menghubungkan ke server menggunakan connect. Jika gagal, program keluar dengan pesan kesalahan.
    4. Menerima Pesan Awal: Menerima pesan awal dari server menggunakan recv dan mencetaknya.
    5. Mendengarkan Pesan: Loop terus-menerus untuk menerima pesan dari server dan mencetaknya. Jika koneksi terputus, keluar dari loop dan menutup socket.

Fungsi main:

    - Memeriksa Argumen: Memeriksa argumen yang diberikan untuk memastikan format yang benar (-channel channel_name -room room_name).
    - Menangkap Nama Saluran dan Ruang: Menangkap nama saluran dan ruang dari argumen.
    - Menghubungkan ke Server: Memanggil fungsi connect_to_server untuk menghubungkan ke server.

Lalu setelah itu diharapkan kita dapat melihat chat yang dilakukan oleh client secara real time 

##### Fungsi yang berhasil 
1. REGISTER
2. LOGIN
3. CREATE CHANNEL
4. DEL CHANNEL
5. LOG ACTIVITY 
6. EDIT CHANNEL 
7. LIST CHANNEL 

##### Fungsi yang belum berhasil 
1. JOIN CHANNEL 
2. JOIN ROOM 
3. CREATE ROOM 
4. CHAT
5. DEL ROOM 
6. LIST USERS 
7. LIST ROOM
8. EDIT USERNAME 
9. REMOVE USER 
10. BAN USER 
11. UNBAN USER 
12. Monitor 

## Pekerjaan (Revisi)
### discorit.c 
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_BUFFER 1024

int Conect;


void initiate_connection() {
    struct sockaddr_in server_addr;

    // Membuat socket
    if ((Conect = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Gagal membuat socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Alamat tidak valid atau tidak didukung");
        exit(EXIT_FAILURE);
    }

    // Menghubungkan ke server
    if (connect(Conect, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Koneksi gagal");
        exit(EXIT_FAILURE);
    }
}

// Fungsi untuk menjalankan perintah dari pengguna
void execute_command(const char *cmd, char *user, char *chan, char *rm) {
    if (!cmd || cmd[0] == '\0') {
        printf("Perintah tidak boleh kosong\n");
        return;
    }

    // Mengirim perintah ke server
    if (send(Conect, cmd, strlen(cmd), 0) < 0) {
        perror("Gagal mengirim perintah ke server");
    }

    char server_reply[MAX_BUFFER];
    memset(server_reply, 0, sizeof(server_reply));

    // Menerima respons dari server
    if (recv(Conect, server_reply, MAX_BUFFER - 1, 0) < 0) {
        perror("Gagal menerima respons dari server");
    } else {
        if (strstr(server_reply, "Key:") != NULL) {
            char key_input[50];
            printf("Key: ");
            fgets(key_input, sizeof(key_input), stdin);
            key_input[strcspn(key_input, "\n")] = '\0'; // Menghapus karakter newline

            if (send(Conect, key_input, strlen(key_input), 0) < 0) {
                perror("Gagal mengirim key ke server");
            }

            // Menerima respons setelah mengirimkan key
            memset(server_reply, 0, sizeof(server_reply));
            if (recv(Conect, server_reply, MAX_BUFFER - 1, 0) < 0) {
                perror("Gagal menerima respons dari server");
            } else {
                if (strstr(server_reply, "Invalid key") != NULL) {
                    // Reset state jika key tidak valid
                    if (strlen(rm) > 0) {
                        rm[0] = '\0';
                    } else if (strlen(chan) > 0) {
                        chan[0] = '\0';
                    }
                }
            }
        } else if (strstr(server_reply, "not found") != NULL || strstr(server_reply, "Invalid key") != NULL || strstr(server_reply, "You have been banned") != NULL) {
            if (strlen(rm) > 0) {
                rm[0] = '\0';
            } else if (strlen(chan) > 0) {
                chan[0] = '\0';
            }
            printf("%s\n", server_reply);
        } else if (strstr(server_reply, "You have exited the application") != NULL) {
            close(Conect);
            exit(0);  // Keluar dari program klien setelah menerima konfirmasi keluar
        } else {
            printf("%s\n", server_reply);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Penggunaan: %s REGISTER/LOGIN username -p password\n", argv[0]);
        return 1;
    }

    initiate_connection();

    char cmd[MAX_BUFFER];
    memset(cmd, 0, sizeof(cmd));

    char user[50];
    char chan[50] = "";
    char rm[50] = "";

    if (strcmp(argv[1], "REGISTER") == 0) {
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Penggunaan: %s REGISTER username -p password\n", argv[0]);
            return 1;
        }

        snprintf(user, sizeof(user), "%s", argv[2]);
        char *pwd = argv[4];

        snprintf(cmd, sizeof(cmd), "REGISTER %s %s", user, pwd);
    } else if (strcmp(argv[1], "LOGIN") == 0) {
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Penggunaan: %s LOGIN username -p password\n", argv[0]);
            return 1;
        }

        snprintf(user, sizeof(user), "%s", argv[2]);
        char *pwd = argv[4];

        snprintf(cmd, sizeof(cmd), "LOGIN %s %s", user, pwd);

        if (send(Conect, cmd, strlen(cmd), 0) < 0) {
            perror("Gagal mengirim perintah ke server");
        }

        char server_reply[MAX_BUFFER];
        memset(server_reply, 0, sizeof(server_reply));

        if (recv(Conect, server_reply, MAX_BUFFER - 1, 0) < 0) {
            perror("Gagal menerima respons dari server");
        } else {
            printf("%s\n", server_reply);

            if (strstr(server_reply, "successfully logged in") != NULL) {
                while (1) {
                    if (strlen(rm) > 0) {
                        printf("[%s/%s/%s] ", user, chan, rm);
                    } else if (strlen(chan) > 0) {
                        printf("[%s/%s] ", user, chan);
                    } else {
                        printf("[%s] ", user);
                    }

                    if (fgets(cmd, MAX_BUFFER, stdin) == NULL) {
                        printf("Gagal membaca perintah\n");
                        continue;
                    }
                    cmd[strcspn(cmd, "\n")] = '\0';

                    // Simpan nama channel atau room sebelum mengirim perintah
                    // Simpan nama pengguna baru ketika diedit
                    if (strncmp(cmd, "JOIN ", 5) == 0) {
                        if (strlen(chan) == 0) {
                            snprintf(chan, sizeof(chan), "%s", cmd + 5);
                        } else {
                            snprintf(rm, sizeof(rm), "%s", cmd + 5);
                        }
                    } else if (strcmp(cmd, "EXIT") == 0) {
                        if (strlen(rm) > 0) {
                            rm[0] = '\0';
                        } else if (strlen(chan) > 0) {
                            chan[0] = '\0';
                        }
                    } else if (strncmp(cmd, "EDIT PROFILE SELF -u ", 21) == 0) {
                        snprintf(user, sizeof(user), "%s", cmd + 21);
                    }

                    execute_command(cmd, user, chan, rm);
                }
            }
        }

        close(Conect);
        return 0;
    } else {
        printf("Perintah tidak valid\n");
        return 1;
    }

    execute_command(cmd, user, chan, rm);

    close(Conect);
    return 0;
}
```
### server.c 
```c

```
### monitor.c 
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>

#define PORT 12345
#define BUF_SIZE 1024

int client_fd;
bool active = true;
char user[50];
char current_channel[50] = "";
char current_room[50] = "";

// Fungsi untuk menghubungkan ke server
void connect_to_server() {
    struct sockaddr_in server_addr;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or unsupported address");
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
}

// Fungsi untuk mengirim perintah ke server dan menangani respons
void send_command(const char *cmd) {
    if (cmd == NULL) {
        printf("Command cannot be empty\n");
        return;
    }

    if (send(client_fd, cmd, strlen(cmd), 0) < 0) {
        perror("Failed to send command to server");
    }

    char response[BUF_SIZE];
    memset(response, 0, sizeof(response));

    if (recv(client_fd, response, BUF_SIZE - 1, 0) < 0) {
        perror("Failed to receive response from server");
    } else {
        if (strstr(response, "Key:") != NULL) {
            char key[50];
            printf("Key: ");
            fgets(key, sizeof(key), stdin);
            key[strcspn(key, "\n")] = '\0';

            if (send(client_fd, key, strlen(key), 0) < 0) {
                perror("Failed to send key to server");
            }

            memset(response, 0, sizeof(response));
            if (recv(client_fd, response, BUF_SIZE - 1, 0) < 0) {
                perror("Failed to receive response from server");
            } else {
                if (strstr(response, "Invalid key") != NULL) {
                    if (strlen(current_room) > 0) {
                        current_room[0] = '\0';
                    } else if (strlen(current_channel) > 0) {
                        current_channel[0] = '\0';
                    }
                }
            }
        } else if (strstr(response, "not found") != NULL || strstr(response, "Invalid key") != NULL || strstr(response, "You have been banned") != NULL || strstr(response, "unknown") != NULL) {
            if (strlen(current_room) > 0) {
                current_room[0] = '\0';
            } else if (strlen(current_channel) > 0) {
                current_channel[0] = '\0';
            }
            printf("%s\n", response);
        } else if (strstr(response, "You have exited the application") != NULL) {
            close(client_fd);
            exit(0);
        } else {
            printf("%s\n", response);
        }
    }
}

// Fungsi untuk menampilkan riwayat chat dari file CSV
void show_chat_history(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char line[BUF_SIZE];
    char output[BUF_SIZE];
    memset(output, 0, sizeof(output));
    bool has_chat = false;

    while (fgets(line, sizeof(line), file)) {
        has_chat = true;
        char *date = strtok(line, "|");
        char *chat_id = strtok(NULL, "|");
        char *sender = strtok(NULL, "|");
        char *message = strtok(NULL, "|");

        if (date && chat_id && sender && message) {
            snprintf(output + strlen(output), sizeof(output) - strlen(output), "[%s][%s][%s] \"%s\"\n", date, chat_id, sender, message);
        }
    }

    fclose(file);

    if (!has_chat) {
        snprintf(output, sizeof(output), "No chat history available\n");
    }

    printf("\033[H\033[J");
    printf("%s", output);
    printf("[%s/%s/%s] ", user, current_channel, current_room);
    fflush(stdout);
}

// Fungsi untuk memonitor perubahan pada file CSV
void *monitor_csv(void *arg) {
    char path[256];
    snprintf(path, sizeof(path), "/home/ubuntu/FP/DiscorIT/%s/%s/chat.csv", current_channel, current_room);
    struct stat file_stat;
    time_t last_mod_time = 0;

    while (active) {
        if (stat(path, &file_stat) == 0) {
            if (file_stat.st_mtime != last_mod_time) {
                last_mod_time = file_stat.st_mtime;
                printf("\033[H\033[J");
                show_chat_history(path);
            }
        } else {
            perror("Failed to get file status");
        }
        sleep(1);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc <= 3) {
        printf("Usage: %s LOGIN username -p password\n", argv[0]);
        return 1;
    }

    connect_to_server();

    char cmd[BUF_SIZE];
    memset(cmd, 0, sizeof(cmd));

    if (strcmp(argv[1], "LOGIN") == 0) {
        if (argc < 5 || strcmp(argv[3], "-p") != 0) {
            printf("Usage: %s LOGIN username -p password\n", argv[0]);
            return 1;
        }

        snprintf(user, sizeof(user), "%s", argv[2]);
        char *password = argv[4];

        snprintf(cmd, sizeof(cmd), "LOGIN %s %s", user, password);

        if (send(client_fd, cmd, strlen(cmd), 0) < 0) {
            perror("Failed to send command to server");
        }

        char response[BUF_SIZE];
        memset(response, 0, sizeof(response));

        if (recv(client_fd, response, BUF_SIZE - 1, 0) < 0) {
            perror("Failed to receive response from server");
        } else {
            printf("%s\n", response);

            if (strstr(response, "successfully logged in") != NULL) {
                while (1) {
                    printf("[%s] ", user);

                    if (fgets(cmd, BUF_SIZE, stdin) == NULL) {
                        printf("Failed to read command\n");
                        continue;
                    }
                    cmd[strcspn(cmd, "\n")] = '\0';

                    char *token = strtok(cmd, " ");
                    if (token == NULL) {
                        printf("Usage: -channel <channel_name> -room <room_name>\n");
                        continue;
                    }

                    if (strcmp(token, "-channel") == 0) {
                        token = strtok(NULL, " ");
                        if (token == NULL) {
                            printf("Usage: -channel <channel_name> -room <room_name>\n");
                            continue;
                        }
                        snprintf(current_channel, sizeof(current_channel), "%s", token);
                        token = strtok(NULL, " ");
                        if(token == NULL){
                            printf("Usage: -channel <channel_name> -room <room_name>\n");
                            continue;
                        }
                        if(strcmp(token, "-room") != 0){
                            printf("Usage: -channel <channel_name> -room <room_name>\n");
                            continue;
                        }
                        token = strtok(NULL, " ");
                        if(token == NULL){
                            printf("Usage: -channel <channel_name> -room <room_name>\n");
                            continue;
                        }
                        snprintf(current_room, sizeof(current_room), "%s", token);

                        pthread_t csv_thread;
                        active = true;
                        if (pthread_create(&csv_thread, NULL, monitor_csv, NULL) != 0) {
                            perror("Failed to create thread to monitor CSV");
                            continue;
                        }

                        pthread_detach(csv_thread);

                        char path[256];
                        snprintf(path, sizeof(path), "/home/ubuntu/FP/DiscorIT/%s/%s/chat.csv", current_channel, current_room);
                        show_chat_history(path);
                    } else if (strcmp(token, "EXIT") == 0) {
                        if (strlen(current_room) > 0 || strlen(current_channel) > 0) {
                            current_room[0] = '\0';
                            current_channel[0] = '\0';
                            active = false;
                            printf("\033[H\033[J");
                        } else {
                            send_command(cmd);
                            break;
                        }
                    } else {
                        printf("Usage: -channel <channel_name> -room <room_name>\nIf you want to exit, use the EXIT command\n");
                    }
                }
            }
            close(client_fd);
            return 0;
        }
    } else {
        printf("Usage: %s LOGIN username -p password\n", argv[0]);
        return 1;
    }
    close(client_fd);
    return 0;
}

```
#### Penjelasan Singkat (Revisi)
discorit.c 

Pada revisi discorit pertama yang diubah adalah discorit.c yang tadinya belum bisa menampilkan [username/channel/room] sehingga bisa menampilkannya dan juga memberikan fungsi untuk EXIT tidak langsung keluar dari program namun mulai dari room, channel, baru keluar dari program  

server.c 

Pada revisi server.c cukup banyak yang diubah mulai dari cara join channelnya, edit profile, lalu cukup banyak juga fungsi yang ditambahkan seperti ban, unban, chat, see chat, remove user, dan untuk yang sebelumnya tidak bisa seperti list room, dan list users hal ini berkaitan dengan tidak bisanya join channel meskipun cara list itu sama 

monitor.c 

Pada revisi monitor.c kami sudah mengupayakan agar monnitor bisa masuk ke akun user dan bisa melihat chat secara realtime namun ternyata monitor masih tidak bisa login user 


#### Hasil revisi 
- Hasil dari REGISTER, LOGIN, CREATE CHANNEL, JOIN, CREATE ROOM. 
![CREATE CHANNEL dan ROOM](https://github.com/RafaelJonathanA/Sisop/assets/150375098/4d6c0f7e-324f-4d23-aa61-7e119bc5ce07)

- Hasil dari LIST CHANNEL, LIST USER 
![LIST CHANNEL dan ROOM](https://github.com/RafaelJonathanA/Sisop/assets/150375098/010d537f-0712-48ca-88a3-19ee2640f63f)

- Hasil dari CHAT, SEE CHAT, EDIT CHAT 
![chat rafa](https://github.com/RafaelJonathanA/Sisop/assets/150375098/d3472912-2700-4369-b1c8-eea72fd55249)

- Hasil dari CHAT, SEE CHAT, DEL CHAT, EDIT CHAT 
![chat fidel](https://github.com/RafaelJonathanA/Sisop/assets/150375098/723335ac-0c13-43f3-b6ff-60a990ac00c4)

- Hasil EDIT CHANNEL dan DEL CHANNEL 
![EDIT CHANNEL dan DEL CHANNEL](https://github.com/RafaelJonathanA/Sisop/assets/150375098/f8671699-bd06-4887-8ad9-b5014531c738)

- Hasil EDIT user (ROOT) dan REMOVE 
![Kuasa ROOT](https://github.com/RafaelJonathanA/Sisop/assets/150375098/dbf384f0-a08c-4597-a38c-3435430315c4)

- Hasil pada username fidel
![Bukti Kuasa Root](https://github.com/RafaelJonathanA/Sisop/assets/150375098/918c46c6-abd7-4ce8-ac75-81aa70796e6f)

- Hasil dari BAN, UNBAN
![Ban dan UNBAN](https://github.com/RafaelJonathanA/Sisop/assets/150375098/1cfaa5aa-002a-45ea-bf12-ae61433c59af)

- Hasil pada username fidel 
![Bukti ke ban ](https://github.com/RafaelJonathanA/Sisop/assets/150375098/6b1c378a-7538-453a-9a11-5e7780dcd562)

- Hasil EDIT profile diri sendiri 
![Bukti update diri sendiri](https://github.com/RafaelJonathanA/Sisop/assets/150375098/4e8c76f0-e920-419c-ab71-fdb6d175a949)

- Hasil EXIT 
![EXIT](https://github.com/RafaelJonathanA/Sisop/assets/150375098/dba59328-194e-4980-9ef0-0fea38f29913)

##### Fungsi yang berhasil setelah revisi 
1. JOIN CHANNEL 
2. JOIN ROOM 
3. CREATE ROOM 
4. CHAT
5. DEL ROOM 
6. LIST USERS 
7. LIST ROOM
8. EDIT USERNAME 
9. REMOVE USER 
10. BAN USER 
11. UNBAN USER

##### Fungsi yang belum berhasil setelah revisi 
1. Monitor 
