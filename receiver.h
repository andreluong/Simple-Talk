#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include "list.h"

void Receiver_init(List *list, int socket, struct sockaddr_in addr);
void Receiver_cancel();
void Receiver_shutdown();

#endif