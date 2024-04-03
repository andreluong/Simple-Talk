#ifndef _LIST_OPERATIONS_H_
#define _LIST_OPERATIONS_H_

#include "list.h"

int List_prepend_with_mutex(List* pList, void* pItem);
void* List_trim_with_mutex(List* pList);

#endif