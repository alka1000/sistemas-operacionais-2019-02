/**
 * Snippet de código pego de https://codereview.stackexchange.com/a/128201
 * Modificado para melhor se adaptar as necessidades do codigo.
 *
 *  Leonardo Reis - Amir Leonardo
 * 
 *  */

#include "queue.h"

bool initQueue(Queue* pQueue)
{
    if (!pQueue)
    {
        return false;
    }

    pQueue->head = 0;
    pQueue->tail = 0;
    pQueue->size = 0;
    return true;
}

int isEmpty(Queue* pQueue)
{
    return pQueue && pQueue->size == 0;
}

bool enqueue(Queue* pQueue, void* item)
{
    if (!pQueue || pQueue->size == QUEUE_CAPACITY) // when queue is full
    {
        return false;
    }

    pQueue->data[pQueue->tail] = item;
    pQueue->tail = (pQueue->tail + 1) % QUEUE_CAPACITY;
    pQueue->size++;
    return true;
}

void* dequeue(Queue* pQueue)
{
    int i;
    // Return NULL when queue is empty
    // Return (void*)item at the head otherwise.
    void* item;

    if (!pQueue || isEmpty(pQueue))
    {
        return NULL;
    }

    item = pQueue->data[pQueue->head];
    pQueue->data[pQueue->head] = NULL;
    //pQueue->head = (pQueue->head + 1) % QUEUE_CAPACITY;
    for(i=0;i<(pQueue->size)-1;i++){
        pQueue->data[i] = pQueue->data[i+1];
        pQueue->data[i+1] = NULL;
    }
    pQueue->tail--;
    pQueue->size--;
    return item;
}

int has_task (Queue* pQueue, void* task) {
    int length = pQueue->size;
    int i;
    for (i = pQueue->head; i < length ; i++) {
      if (pQueue->data[i] == task) {
        return 1;
      }
    }
    return 0;
}


void *remove_task (Queue* pQueue, int i) {

    void* item;

    if (!pQueue || isEmpty(pQueue))
    {
        return NULL;
    }

    item = pQueue->data[i];

    pQueue->data[i] = NULL;

    for(int j=i;j<(pQueue->size)-1;j++){
        pQueue->data[j] = pQueue->data[j+1];
        pQueue->data[j+1] = NULL;
    }

    pQueue->tail--;
    pQueue->size--;
    return item;
}

void swap(void *a, void *b) {
  void *temp;
  temp = a;
  a = b;
  b = temp;
}

