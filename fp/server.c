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
#define CSV_FILE "/home/ubuntu/FP/DiscorIT/users.csv"
#define BASE_DIR "/home/ubuntu/FP/DiscorIT"
#define CHANNELS_FILE "/home/ubuntu/FP/DiscorIT/channels.csv"
#define LOG_FILE "/home/ubuntu/FP/DiscorIT/users.log"
#define AUTH_FILE "/home/ubuntu/FP/DiscorIT/auth.csv"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int clients[MAX_CLIENTS];

typedef struct {
    int id_user;
    char name[50];
    char password[100];
    char global_role[10];
} User;

void *handle_client(void *arg);
void register_user(int client_socket, char *username, char *password);
void login_user(int client_socket, char *username, char *password);
void list_channels(int client_socket);
void list_rooms(int client_socket, char *channel);
void list_users(int client_socket, char *channel);
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

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;

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

    bzero(buffer, BUFFER_SIZE);

    while (read(client_socket, buffer, BUFFER_SIZE - 1) > 0) {
        sscanf(buffer, "%s %s -p %s", command, username, password);
        if (strcmp(command, "REGISTER") == 0) {
            register_user(client_socket, username, password);
        } else if (strcmp(command, "LOGIN") == 0) {
            login_user(client_socket, username, password);
            bzero(buffer, BUFFER_SIZE);
            while (read(client_socket, buffer, BUFFER_SIZE - 1) > 0) {
                if (sscanf(buffer, "%s CREATE CHANNEL %s -k %s", username, channel, key) == 3) {
                    create_channel(client_socket, username, channel, key);
                } else if (sscanf(buffer, "%s EDIT CHANNEL %s TO %s", username, channel, new_name) == 3) {
                    edit_channel(client_socket, channel, new_name);
                } else if (sscanf(buffer, "%s DEL CHANNEL %s", username, channel) == 2) {
                    delete_channel(client_socket, channel);
                } else if (sscanf(buffer, "%s/%s CREATE ROOM %s", username, channel, room) == 3) {
                    create_room(client_socket, channel, room);
                } else if (sscanf(buffer, "%s/%s EDIT ROOM %s TO %s", username, channel, room, new_name) == 4) {
                    edit_room(client_socket, channel, room, new_name);
                } else if (sscanf(buffer, "%s/%s DEL ROOM %s", username, channel, room) == 3) {
                    delete_room(client_socket, channel, room);
                } else if (sscanf(buffer, "%s/%s DEL ROOM ALL", username, channel) == 2) {
                    delete_all_rooms(client_socket, channel);
                } else if (strncmp(buffer, "LIST CHANNEL", 12) == 0) {
                    list_channels(client_socket);
                } else if (sscanf(buffer, "%s JOIN %s", username, channel) == 2) {
                    sscanf(buffer, "%s JOIN %s Key: %s", username, channel, key);
                    join_channel(client_socket, username, channel, key);
                } else if (sscanf(buffer, "%s/%s JOIN %s", username, channel, room) == 3) {
                    join_room(client_socket, username, channel, room);
                } else if (sscanf(buffer, "%s/%s LIST ROOM", username, channel) == 2) {
                    list_rooms(client_socket, channel);
                } else if (sscanf(buffer, "%s/%s LIST USER", username, channel) == 2) {
                    list_users(client_socket, channel);
                } else {
                    write(client_socket, "ERROR: Invalid command\n", 23);
                }
                bzero(buffer, BUFFER_SIZE);
            }
        } else {
            write(client_socket, "ERROR: Invalid command\n", 23);
        }
        bzero(buffer, BUFFER_SIZE);
    }

    close(client_socket);
    pthread_exit(NULL);
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

    bzero(buffer, BUFFER_SIZE);
    while (fscanf(fp, "%49[^,],%49[^\n]\n", channel, key) != EOF) {
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
    while (fscanf(fp, "%49[^,],%49[^\n]\n", stored_channel, stored_key) != EOF) {
        if (strcmp(stored_channel, channel) == 0 && strcmp(stored_key, key) == 0) {
            found = 1;
            break;
        }
    }
    fclose(fp);
    pthread_mutex_unlock(&lock);

    if (!found) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Channel not found or incorrect key\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    snprintf(path, BUFFER_SIZE, "%s/%s", BASE_DIR, channel);
    if (mkdir(path, 0777) != 0 && errno != EEXIST) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Cannot create or access channel directory\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    snprintf(path, BUFFER_SIZE, "%s/%s/%s", BASE_DIR, channel, AUTH_FILE);
    fp = fopen(path, "a+");
    if (fp == NULL) {
        snprintf(buffer, BUFFER_SIZE, "ERROR: Cannot open auth file\n");
        write(client_socket, buffer, strlen(buffer));
        return;
    }

    int id = 1;
    User user;
    while (fscanf(fp, "%d,%49[^,],%99[^,],%9[^\n]\n", &user.id_user, user.name, user.password, user.global_role) != EOF) {
        if (strcmp(user.name, username) == 0) {
            fclose(fp);
            snprintf(buffer, BUFFER_SIZE, "Welcome back to channel %s\n", channel);
            write(client_socket, buffer, strlen(buffer));
            return;
        }
        id++;
    }

    fprintf(fp, "%d,%s,%s\n", id, username, "-");
    fclose(fp);

    snprintf(buffer, BUFFER_SIZE, "Welcome to channel %s\n", channel);
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
