#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "style.h"
#include "diffiehellman.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int new_socket = 0;
char buffer[BUFFER_SIZE] = {0};
void *receive_messages();
void server_DiffieHellmanHandshake(int new_socket);
void server_test(int new_socket);

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    //server_DiffieHellmanHandshake(new_socket);
    server_test(new_socket);
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    char message[BUFFER_SIZE];
    while (1) {
        changeTextblue();
        printf("Enter message: ");
        fgets(message, BUFFER_SIZE, stdin);
        send(new_socket, message, strlen(message), 0);
        if (strcmp(message, "exit\n") == 0) {
            printf("Connection Ended");
            break;
        }
    }

    pthread_join(recv_thread, NULL);
    close(new_socket);
    close(server_fd);

    return 0;
}

// Function to handle receiving messages from client
void *receive_messages() {
    while (1) {
        int valread = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            break;
        }
        changeTextRed();
        printf("\nClient: %s", buffer);
        changeTextblue();
        printf("Enter message: ");
        fflush(stdout);
        memset(buffer, 0, BUFFER_SIZE);
    }
    pthread_exit(NULL);
}

void server_DiffieHellmanHandshake(int new_socket){
    uint64_t shared_secret;
    DH_Params dh_params;
    recv(new_socket, &dh_params, sizeof(DH_Params), 0);
    printf("params received: %lu %lu\n", dh_params.g , dh_params.p);
    uint64_t priv_key = generate_private_key(dh_params.p-1);
    printf("private key: %lu\n", priv_key);
    uint64_t server_pub_key = generate_public_key(dh_params, priv_key);
    send(new_socket, &server_pub_key, sizeof(uint64_t), 0);
    uint64_t client_pub_key;
    recv(new_socket, &client_pub_key, sizeof(uint64_t), 0);
    printf("client public key: %lu\n", client_pub_key);
    shared_secret = compute_shared_secret(dh_params, client_pub_key,priv_key);
    changeTextItalic();
    printf("Shared Diffie Hellman key: %lu\n", shared_secret);
}

void server_test(int new_socket){
    uint64_t shared_secret;
    DH_Params dh_params;
    recv(new_socket, &dh_params, sizeof(DH_Params), 0);
    printf("params received: %lu %lu\n", dh_params.g , dh_params.p);
    uint64_t priv_key = 13;
    printf("private key: %lu\n", priv_key);
    uint64_t server_pub_key = generate_public_key(dh_params, priv_key);
    send(new_socket, &server_pub_key, sizeof(uint64_t), 0);
    uint64_t client_pub_key;
    recv(new_socket, &client_pub_key, sizeof(uint64_t), 0);
    printf("client public key: %lu\n", client_pub_key);
    shared_secret = compute_shared_secret(dh_params, client_pub_key,priv_key);
    changeTextItalic();
    printf("Shared Diffie Hellman key: %lu\n", shared_secret);
}