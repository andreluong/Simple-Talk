#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include "receiver.h"
#include "printer.h"
#include "keyboard.h"
#include "sender.h"
#include "listOperations.h"

#define MAX_LEN 1024
#define REMOTE_MSG "Remote: "

static int sockfd;
static struct sockaddr_in remote;
static unsigned int remoteLen;
static pthread_t receiverThread;
static List *outputList;

// Receives a message sent from remote and prepends it to the output list
void* receiveThreadFunction() {
    while (true) {
        char *message, buffer[MAX_LEN];
        int bufferLen;

        // Receive buffer message
        if ((bufferLen = recvfrom(sockfd, buffer, MAX_LEN, 0, (struct sockaddr *) &remote, &remoteLen)) == -1) {
            perror("ERROR: receiver can't receive message\n");
            exit(EXIT_FAILURE);
        }

        // Puts buffer into message
        bufferLen += strlen(REMOTE_MSG);
        message = (char *)malloc(bufferLen + 1);
        strcpy(message, REMOTE_MSG);
        strncat(message, buffer, bufferLen);
        message[bufferLen] = '\0';

        // Prepend message to output list
        if (List_prepend_with_mutex(outputList, message) == LIST_FAIL) {
            perror("ERROR: can't prepend message to output list\n");
            exit(EXIT_FAILURE);
        }

        // Check for user termination being received
        if (strcmp(message, "Remote: !\n") == 0) {
            Printer_signal();
            // Interrupts remaining threads (keyboard and sender) to terminate the program for both clients
            // Printer thread will be closed beforehand
            // This thread (receiver) will close after 
            Keyboard_cancel();
            Sender_cancel();
            break;
        }
        Printer_signal();
    }
    return NULL;
}

// Cancel receiver thread
void Receiver_cancel() {
    int resultCode;
    if ((resultCode = pthread_cancel(receiverThread)) != 0) {
        printf("ERROR: receiver can't cancel thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}

// Create receiver thread
void Receiver_init(List *list, int socket, struct sockaddr_in addr) {
    outputList = list;
    sockfd = socket;
    remote = addr;
    remoteLen = sizeof(remote);
    int resultCode;
    if ((resultCode = pthread_create(&receiverThread, NULL, receiveThreadFunction, NULL)) != 0) {
        printf("ERROR: receiver can't create thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}

// Join receiver thread to end
void Receiver_shutdown() {
    int resultCode;
    if ((resultCode = pthread_join(receiverThread, NULL)) != 0) {
        printf("ERROR: receiver can't join thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}