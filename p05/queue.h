#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdio.h>
#include <ucontext.h>
#include "datatypes.h"

#define QUEUE_CAPACITY 500

typedef struct {
    task_t* data[QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t size;
} Queue;

bool initQueue(Queue* pQueue);

int isEmpty(Queue* pQueue);
bool enqueue(Queue* pQueue, task_t* item);

task_t* dequeue(Queue* pQueue);

void bubble_sort (Queue* pQueue);

void swap(task_t *a, task_t *b);

void debugPrint(Queue* pQueue);

#endif
