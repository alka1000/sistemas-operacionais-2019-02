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
    // Return NULL when queue is empty
    // Return (void*)item at the head otherwise.
    void* item;

    if (!pQueue || isEmpty(pQueue))
    {
        return NULL;
    }

    item = pQueue->data[pQueue->head];
    pQueue->head = (pQueue->head + 1) % QUEUE_CAPACITY;
    pQueue->size--;
    return item;
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
        printf("%d", (int) pQueue->data[pQueue->head]);
    }

    for (index = 1; index < pQueue->size; ++index)
    {
        tmp = (pQueue->head + index) % QUEUE_CAPACITY;
        printf(", %d", (int) pQueue->data[tmp]);
    }

    printf("]");
}