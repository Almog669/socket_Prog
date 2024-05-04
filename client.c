#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "Style.h"
#include "diffiehellman.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int sock = 0;
uint64_t shared_secret;
char buffer[BUFFER_SIZE] = {0};

// Function to handle receiving messages from server
void *receive_messages() {
    while (1) {
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            break;
        }
        changeTextRed();
        printf("\nServer: %s", buffer);
        changeTextblue();
        printf("Enter message: ");
        fflush(stdout);
        memset(buffer, 0, BUFFER_SIZE);
    }
    pthread_exit(NULL);
}

int main() {
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    DH_Params dh_params = generate_dh_params();
    send(sock, &dh_params, sizeof(DH_Params), 0);
    uint64_t server_pub_key = generate_public_key(dh_params);
    send(sock, &server_pub_key, sizeof(uint64_t), 0);
    shared_secret = compute_shared_secret(dh_params, server_pub_key);
    changeTextItalic();
    printf("Shared Diffie Hellman key: %lu\n", shared_secret);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    char message[BUFFER_SIZE];
    while (1) {
        changeTextblue();
        printf("Enter message: ");
        fgets(message, BUFFER_SIZE, stdin);
        send(sock, message, strlen(message), 0);
        if (strcmp(message, "exit\n") == 0) {
            printf("Connection Ended");
            break;
        }
    }

    pthread_join(recv_thread, NULL);
    close(sock);
    return 0;
}
