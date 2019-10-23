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

bool enqueue(Queue* pQueue, task_t* item)
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

task_t* dequeue(Queue* pQueue)
{
    int i;
    // Return NULL when queue is empty
    // Return (task_t*)item at the head otherwise.
    task_t* item;

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

void bubble_sort (Queue* pQueue) {
    int length = pQueue->size;
    int i, j;
    for (i = pQueue->head; i < length - 1; i++) {
        for (j = pQueue->head; j < length - i - 1; j++) {
          if (pQueue->data[j]->priority > pQueue->data[j+1]->priority) {
            swap(pQueue->data[j], pQueue->data[j+1]);
          }
        }
    }
}

int has_task (Queue* pQueue, task_t* task) {
    int length = pQueue->size;
    int i;
    for (i = pQueue->head; i < length ; i++) {
      if (pQueue->data[i] == task) {
        return 1;
      }
    }
    return 0;
}


task_t *remove_task (Queue* pQueue, int i) {

    task_t* item;

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

void swap(task_t *a, task_t *b) {
  task_t temp;
  temp = *a;
  *a = *b;
  *b = temp;
}

void debugPrint(Queue* pQueue)
{
    size_t index;
    size_t tmp;

    if (!pQueue)
    {
        printf("null");
        return;
    }

    printf("[");

    if (pQueue->size >= 1)
    {
        printf("%d", (int) pQueue->data[pQueue->head]->id);
    }

    for (index = 1; index < pQueue->size; ++index)
    {
        tmp = (pQueue->head + index) % QUEUE_CAPACITY;
        printf(", %d", (int) pQueue->data[tmp]->id);
    }

    printf("]");
}