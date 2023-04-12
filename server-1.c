#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SZ 2048

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

/* Client structure */
typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[32];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void queue_add(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void queue_delete(int uid) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->uid == uid) {
            clients[i] = NULL;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void send_message(char *s, int uid) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->uid == uid) {
            send(clients[i]->sockfd, s, strlen(s), 0);
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_all(char *s, int uid) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->uid != uid) {
            send(clients[i]->sockfd, s, strlen(s), 0);
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    char buffer[BUFFER_SZ];
    char name[32];
    int leave_flag = 0;

    cli_count++;
    client_t *cli = (client_t *)arg;

    // Name
    if (recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1) {
        printf("Didn't enter the name.\n");
        leave_flag = 1;
    } else {
        strcpy(cli->name, name);
        sprintf(buffer, "%s has joined\n", cli->name);
        printf("%s", buffer);
        send_message_to_all(buffer, cli->uid);
    }

    bzero(buffer, BUFFER_SZ);

    while (1) {
        if (leave_flag) {
            break;
        }

        int receive = recv(cli->sockfd, buffer, BUFFER_SZ, 0);

        if (receive > 0) {
            if (strlen(buffer) > 0) {
                if (buffer[0] == '@') {
                    // Extract recipient's name and message
                    char *recipient = strtok(buffer, " ");
                    char *message = strtok(NULL, "\n");
                    if (message != NULL) {
                        // Remove '@' from recipient's name
                        memmove(recipient, recipient + 1, strlen(recipient));
                        recipient[strlen(recipient)] =

