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
