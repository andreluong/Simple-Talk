// List operations with mutexes to prevent concurrent access
#include "listOperations.h"
#include <pthread.h>

static pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;

int List_prepend_with_mutex(List* pList, void* pItem) {
    int result;
    pthread_mutex_lock(&listMutex);
    {
        result = List_prepend(pList, pItem);
    }
    pthread_mutex_unlock(&listMutex);
    return result;
}

void* List_trim_with_mutex(List* pList) {
    void* result;
    pthread_mutex_lock(&listMutex);
    {
        result = List_trim(pList);
    }
    pthread_mutex_unlock(&listMutex);
    return result;
}