#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// Structure to pass data to the threads
struct ThreadData {
    char *inputString;
};

// Thread function to change lowercase 'a' to uppercase 'A'
void* modifyA(void* arg) {
    struct ThreadData *threadData = (struct ThreadData*)arg;
    char *inputString = threadData->inputString;

    for (int i = 0; i < strlen(inputString); ++i) {
        if (inputString[i] == 'a') {
            inputString[i] = 'A';
        }
    }

    pthread_exit(NULL);
}

// Thread function to change lowercase 'e' to uppercase 'E'
void* modifyE(void* arg) {
    struct ThreadData *threadData = (struct ThreadData*)arg;
    char *inputString = threadData->inputString;

    for (int i = 0; i < strlen(inputString); ++i) {
        if (inputString[i] == 'e') {
            inputString[i] = 'E';
        }
    }

    pthread_exit(NULL);
}

int main() {
    // Input string
    char inputString[100];

    printf("Enter a string: ");
    fgets(inputString, sizeof(inputString), stdin);

    // Create thread data structures
    struct ThreadData threadDataA, threadDataE;
    threadDataA.inputString = inputString;
    threadDataE.inputString = inputString;

    // Create threads
    pthread_t threadA, threadE;

    if (pthread_create(&threadA, NULL, modifyA, (void*)&threadDataA) != 0 ||
        pthread_create(&threadE, NULL, modifyE, (void*)&threadDataE) != 0) {
        fprintf(stderr, "Error creating thread\n");
        exit(EXIT_FAILURE);
    }

    // Wait for the threads to finish
    pthread_join(threadA, NULL);
    pthread_join(threadE, NULL);

    // Print the modified string
    printf("Modified String: %s", inputString);

    return 0;
}
