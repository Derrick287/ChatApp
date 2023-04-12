

#include <stdio.h>    // For standard input/output functions
#include <stdlib.h>   // For memory allocation and other utility functions
#include <string.h>   // For string manipulation functions
#include <sys/socket.h>  // For socket functions
#include <netinet/in.h>  // For internet address structures
#include <unistd.h>   // For POSIX API functions
#include <arpa/inet.h>   // For internet address conversion functions
#include <pthread.h>  // For POSIX threads

#define PORT 9000
#define BUF_SIZE 1024
#define MAX_CLIENTS 5

int client_sockets[MAX_CLIENTS]; // Array to store client sockets
int num_clients = 0; // Number of connected clients

void *handle_client(void *arg);

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("socket bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, MAX_CLIENTS) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct sockaddr_in client_address;
        int client_socket;
        socklen_t client_address_len;
        client_address_len = sizeof(client_address);
        client_socket = accept(sockfd, (struct sockaddr *)&client_address, &client_address_len);

        if (client_socket < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        client_sockets[num_clients++] = client_socket; // Add client socket to array
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, (void *)&client_socket) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUF_SIZE];
    int read_size;
    
    while ((read_size = recv(client_socket, buffer, BUF_SIZE, 0)) > 0) {
        buffer[read_size] = '\0'; // Add null terminator
        printf("Received message: %s\n", buffer);
        // Broadcast message to all clients except the sender
        for (int i = 0; i < num_clients; i++) {
            if (client_sockets[i] != client_socket) {
                send(client_sockets[i], buffer, strlen(buffer), 0);
            }
        }
    }
    
    if (read_size == 0) {
        puts("Client disconnected");
    } else if (read_size == -1) {
        perror("recv failed");
    }
    
    close(client_socket);
    pthread_exit(NULL);
}

