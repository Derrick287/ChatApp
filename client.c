
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define SERVER_ADDR "127.0.0.1" // server IP address
#define SERVER_PORT 9000 // server port number
#define BUFFER_SIZE 1024 // buffer size for messages
int sock; // socket descriptor
struct sockaddr_in server_addr; // server address structure
char username[20]; // variable to store username
void connect_server() {
    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);

    // connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }
}
void login(char *user) {
    sprintf(username, "%s", user);
    // send login command to server
    char login_msg[BUFFER_SIZE];
    sprintf(login_msg, "login %s", user);
    send(sock, login_msg, strlen(login_msg), 0);
}
void logout() {
    // send logout command to server
    char logout_msg[BUFFER_SIZE];
    sprintf(logout_msg, "logout");
    send(sock, logout_msg, strlen(logout_msg), 0);
    // reset username
    memset(username, 0, sizeof(username));
}
void chat(char *msg) {
    char chat_msg[BUFFER_SIZE];
    if (msg[0] == '@') {
        // send private message to specified user
        char *to_user = strtok(msg, " ");
        to_user++;
        sprintf(chat_msg, "chat %s [%s] %s", to_user, username, strtok(NULL, ""));
    } else {
        // broadcast message to all users
        sprintf(chat_msg, "chat [%s] %s", username, msg);
    }
    send(sock, chat_msg, strlen(chat_msg), 0);
}
int main() {
    char command[BUFFER_SIZE];
    connect_server();

    while (1) {
        printf("Enter command: ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = 0; // remove newline character from input

        if (strcmp(command, "exit") == 0) {
            if (strlen(username) > 0) {
                // send logout command before exiting
                logout();
            }
            break;
        } else if (strncmp(command, "login ", 6) == 0) {
            char *user = strtok(command, " ");
            user = strtok(NULL, "");
            login(user);
        } else if (strcmp(command, "logout") == 0) {
            if (strlen(username) > 0) {
                logout();
            } else {
                printf("You are not logged in.\n");
            }
        } else if (strncmp(command, "chat", 4) == 0) {
            if (strlen(username) > 0) {
                char *msg = strtok(command, " ");
                char *user = strtok(NULL, " ");
                if (user == NULL) {
                    printf("Broadcast message: %s\n", msg + 5); // print message without "chat "
                    chat(msg + 5);
                } else {
                    printf("Private message to %s: %s\n", user, msg + strlen(user) + 6); // print message without "chat @username "
                    chat(msg + strlen(user) + 6);
                }
            } else {
                printf("You need to log in to chat.\n");
            }
        } else {
            printf("Invalid command.\n");
        }
    }

    close(sock);

    return 0;
}
