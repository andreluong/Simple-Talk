#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sys/socket.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "receiver.h"
#include "sender.h"
#include "printer.h"
#include "keyboard.h"
#include "list.h"

#define MAX_ARGS 4

static char *myPort, *remoteHost, *remotePort;
static int sockfd;
static struct addrinfo *servinfo;
static struct sockaddr_in remote;

void setConnectionVars(int argc, char** argv) {
    if (argc != MAX_ARGS) {
        fprintf(stderr, "usage: ./simple-talk [my port number] [remote machine ip address] [remote port number]\n");
        exit(EXIT_FAILURE);
    }
    myPort = argv[1];
    remoteHost = argv[2];
    remotePort = argv[3];
}

void initServerInfo() {
    struct addrinfo hints;
    int status;

    // Build hints struct
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, myPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
}

void initSocket() {
    struct addrinfo *p;

   // loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("ERROR: server: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("ERROR: server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo); // all done with this structure 
}

void initRemote() {
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(atoi(remotePort));
    if (inet_pton(AF_INET, remoteHost, &remote.sin_addr.s_addr) == -1) {
        perror("ERROR: Invalid address found for remote");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    List *inputList = List_create();
    List *outputList = List_create();

    setConnectionVars(argc, argv);
    initServerInfo();
    initSocket();
    initRemote();

    // Threads
    Receiver_init(outputList, sockfd, remote);
    Sender_init(inputList, sockfd, remote);
    Printer_init(outputList);
    Keyboard_init(inputList);

    Receiver_shutdown();
    Sender_shutdown();
    Printer_shutdown();
    Keyboard_shutdown();

    close(sockfd);
    List_free(inputList, free);
    List_free(outputList, free);

    return 0;
}