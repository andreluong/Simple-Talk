#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include "sender.h"
#include "listOperations.h"

#define MAX_LEN 1024

static int sockfd;
static struct sockaddr_in remote;
static pthread_t senderThread;
static List *inputList;

static pthread_cond_t sendCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t sendMutex = PTHREAD_MUTEX_INITIALIZER;

// Blocks the thread until it receives a signal from keyboard
void Sender_waitForSignal() {
    pthread_mutex_lock(&sendMutex);
    {
        pthread_cond_wait(&sendCondVar, &sendMutex);
    }
    pthread_mutex_unlock(&sendMutex);
}

void freeMessage(char *msg) {
    free(msg);
    msg = NULL;
}

// Sends a message from the input list to remote
void* senderThreadFunction() {
    while (true) {
        char *message;

        Sender_waitForSignal();

        // Takes and removes message from list
        if ((message = List_trim_with_mutex(inputList)) == NULL) {
            perror("ERROR: input list is empty");
            freeMessage(message);
            break;
        }

        // Send message to remote
        if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &remote, sizeof(remote)) == -1) {
            perror("ERROR: sender can't send message\n");
            freeMessage(message);
            break;
        }

        // Check for user termination being sent
        if (strcmp(message, "!\n") == 0) {
            freeMessage(message);
            break;
        }
        freeMessage(message);
    }
    return NULL;
}

void Sender_signal() {
    pthread_mutex_lock(&sendMutex);
    {
        pthread_cond_signal(&sendCondVar);
    }
    pthread_mutex_unlock(&sendMutex);
}

// Cancel sender thread
void Sender_cancel() {
    int resultCode;
    if ((resultCode = pthread_cancel(senderThread)) != 0) {
        printf("ERROR: sender can't cancel thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}

// Create sender thread
void Sender_init(List *list, int socket, struct sockaddr_in addr) {
    inputList = list;
    sockfd = socket;
    remote = addr;
    int resultCode;
    if ((resultCode = pthread_create(&senderThread, NULL, senderThreadFunction, NULL)) != 0) {
        printf("ERROR: sender can't create thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}

// Join sender thread to end
void Sender_shutdown() {
    int resultCode;
    if ((resultCode = pthread_join(senderThread, NULL)) != 0) {
        printf("ERROR: sender can't join thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}