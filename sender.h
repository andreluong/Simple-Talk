#ifndef _SENDER_H_
#define _SENDER_H_

#include "list.h"

void Sender_init(List *list, int socket, struct sockaddr_in addr);
void Sender_signal();
void Sender_cancel();
void Sender_shutdown();

#endif