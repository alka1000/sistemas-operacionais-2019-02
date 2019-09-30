/**
 * Snippet de código pego de https://codereview.stackexchange.com/a/128201
 * Modificado para melhor se adaptar as necessidades do codigo.
 *
 * 
 * 
 *  */


#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdio.h>

#define QUEUE_CAPACITY 10

typedef struct {
    void* data[QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t size;
} Queue;

bool initQueue(Queue* pQueue);

int isEmpty(Queue* pQueue);

bool enqueue(Queue* pQueue, void* item);

void* dequeue(Queue* pQueue);

void debugPrint(Queue* pQueue);

#endif
