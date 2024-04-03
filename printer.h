#ifndef _PRINTER_H_
#define _PRINTER_H_

#include "list.h"

void Printer_init(List *list);
void Printer_signal();
void Printer_cancel();
void Printer_shutdown();

#endif