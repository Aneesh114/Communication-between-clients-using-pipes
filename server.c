#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#define PORT 12346

int cl_sk_code1, cl_sk_code2;

void* forward_message(void* cl_sk_code) {
    int* socket = (int*)cl_sk_code;
    char buffer[1024];
    while (1) {
        int read_size = recv(*socket, buffer, sizeof(buffer), 0);
        if (read_size <= 0) {
            close(*socket);
            pthread_exit(NULL);
        }
        send(*socket == cl_sk_code1 ? cl_sk_code2 : cl_sk_code1, buffer, read_size, 0);
    }
    pthread_exit(NULL);
}

int main() {
    int srvr_skt_code;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    srvr_skt_code = socket(AF_INET, SOCK_STREAM, 0);
    if (srvr_skt_code < 0) {
        perror("Error in socket....");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(srvr_skt_code, (struct sockaddr*)&server_addr, sizeof(server_addr) ) < 0) {
        perror("Error in binding....");
        exit(1);
    }

    if (listen(srvr_skt_code, 10) == 0) {
        printf("Server is listening...\n");
    } else {
        printf("Error in listening....");
        exit(1);
    }

    addr_size = sizeof(client_addr);
    cl_sk_code1 = accept(srvr_skt_code, (struct sockaddr*)&client_addr, &addr_size);
    printf("Client 1 connected.\n");

    addr_size = sizeof(client_addr);
    cl_sk_code2 = accept(srvr_skt_code, (struct sockaddr*)&client_addr, &addr_size);
    printf("Client 2 connected.\n");

    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, forward_message, &cl_sk_code1) < 0 || pthread_create(&thread2, NULL, forward_message, &cl_sk_code2) < 0) {
        perror("Error creating threads");
        exit(1);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    close(cl_sk_code1);
    close(cl_sk_code2);
    close(srvr_skt_code);

    return 0;
}