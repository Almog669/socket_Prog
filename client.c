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

int sock = 0;
char buffer[BUFFER_SIZE] = {0};
void *receive_messages();
void client_DiffieHellmanHandshake(int sock);
void client_test(int sock);

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
    
    //client_DiffieHellmanHandshake(sock);
    client_test(sock);
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

void client_DiffieHellmanHandshake(int sock){
    uint64_t shared_secret;
    DH_Params dh_params = generate_dh_params();
    send(sock, &dh_params, sizeof(DH_Params), 0);
    printf("params sent: %lu %lu\n", dh_params.g , dh_params.p);
    uint64_t priv_key = generate_private_key(dh_params.p-1);
    printf("private key: %lu\n", priv_key);
    uint64_t server_public_key;
    recv(sock, &server_public_key, sizeof(uint64_t), 0);
    printf("server public key: %lu\n", server_public_key); 
    uint64_t client_pub_key = generate_public_key(dh_params, priv_key);
    send(sock, &client_pub_key, sizeof(uint64_t), 0); 
    shared_secret = compute_shared_secret(dh_params, server_public_key,priv_key);
    changeTextItalic();
    printf("Shared Diffie Hellman key: %lu\n", shared_secret);
}


void client_test(int sock){
     uint64_t shared_secret;
    DH_Params dh_params;
    dh_params.g = 3;
    dh_params.p = 17;
    send(sock, &dh_params, sizeof(DH_Params), 0);
    printf("params sent: %lu %lu\n", dh_params.g , dh_params.p);
    uint64_t priv_key = 15;
    printf("private key: %lu\n", priv_key);
    uint64_t server_public_key;
    recv(sock, &server_public_key, sizeof(uint64_t), 0);
    printf("server public key: %lu\n", server_public_key); 
    uint64_t client_pub_key = generate_public_key(dh_params, priv_key);
    send(sock, &client_pub_key, sizeof(uint64_t), 0); 
    shared_secret = compute_shared_secret(dh_params, server_public_key,priv_key);
    changeTextItalic();
    printf("Shared Diffie Hellman key: %lu\n", shared_secret);
}