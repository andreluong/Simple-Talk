#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include "keyboard.h"
#include "sender.h"
#include "receiver.h"
#include "printer.h"
#include "listOperations.h"

#define MAX_LEN 1024

static List *inputList;
static pthread_t keyboardThread;

// Get keyboard input from user to prepend to input list
void* keyboardThreadFunction() {
    while (true) {
        char *message, buffer[MAX_LEN];
        int bufferLen;

        // Gets the user input
        if (fgets(buffer, MAX_LEN, stdin) == NULL) {
            perror("ERROR: inputter can't get message");
            exit(EXIT_FAILURE);
        }

        // Copies buffer to message
        bufferLen = strlen(buffer);
        message = (char *)malloc(bufferLen + 1);
        strncpy(message, buffer, bufferLen);
        message[bufferLen] = '\0';

        // Prepend message to input list
        if (List_prepend_with_mutex(inputList, message) == LIST_FAIL) {
            perror("ERROR: can't prepend message to input list");
            exit(EXIT_FAILURE);
        }

        // Check for user termination being inputted
        if (strcmp(message, "!\n") == 0) {
            Sender_signal();
            // Interrupts remaining threads (receiver and printer) to terminate the program for both clients
            // Sender thread will be closed beforehand
            // This thread (keyboard) will close after
            Receiver_cancel();
            Printer_cancel();
            break;
        }
        Sender_signal();
    }
    return NULL;
}

// Cancel keyboard thread
void Keyboard_cancel() {
    int resultCode;
    if ((resultCode = pthread_cancel(keyboardThread)) != 0) {
        printf("ERROR: keyboard can't cancel thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}

// Create keyboard thread
void Keyboard_init(List *list) {
    inputList = list;
    int resultCode;
    if ((resultCode = pthread_create(&keyboardThread, NULL, keyboardThreadFunction, NULL)) != 0) {
        printf("ERROR: keyboard can't create thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}

// Join keyboard thread to end
void Keyboard_shutdown() {
    int resultCode;
    if ((resultCode = pthread_join(keyboardThread, NULL)) != 0) {
        printf("ERROR: keyboard can't join thread. The return code is %d\n", resultCode);
        exit(EXIT_FAILURE);
    }
}