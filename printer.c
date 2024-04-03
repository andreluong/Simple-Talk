#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "printer.h"
#include "listOperations.h"

static List *outputList;
static pthread_t printerThread;

static pthread_cond_t printCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;

// Blocks the thread until it receives a signal from receiver
void Printer_waitForSignal() {
    pthread_mutex_lock(&printMutex);
    {
        pthread_cond_wait(&printCondVar, &printMutex);
    }
    pthread_mutex_unlock(&printMutex);
}

// Takes a message from output list and prints it to the user
void* printerThreadFunction() {
    while (true) {
        char *message;

        Printer_waitForSignal();        

        // Takes and removes message from list
        if ((message = List_trim_with_mutex(outputList)) == NULL) {
            perror("ERROR: output list is empty");
            break;
        }

        // Prints message to screen
        if (fputs(message, stdout) == EOF) {
            perror("ERROR: printer can't print message to screen");
            exit(EXIT_FAILURE);
        }

        // Check for user termination being printed to screen
        if (strcmp(message, "Remote: !\n") == 0) {
            free(message);
            message = NULL;
            break;
        }
        free(message);
        message = NULL;
    }
    return NULL;
}

void Printer_signal() {
    pthread_mutex_lock(&printMutex);
    {
        pthread_cond_signal(&printCondVar);
    }
    pthread_mutex_unlock(&printMutex);
}

// Cancel printer thread
void Printer_cancel() {
    int resultCode;
    if ((resultCode = pthread_cancel(printerThread)) != 0) {
        printf("ERROR: printer can't cancel thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}

// Create printer thread
void Printer_init(List *list) {
    outputList = list;
    int resultCode;
    if ((resultCode = pthread_create(&printerThread, NULL, printerThreadFunction, NULL)) != 0) {
        printf("ERROR: printer can't create thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}

// Join printer thread to end
void Printer_shutdown() {
    int resultCode;
    if ((resultCode = pthread_join(printerThread, NULL)) != 0) {
        printf("ERROR: printer can't join thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}