#ifndef QUEUE_H__
#define QUEUE_H__
#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#endif
#include<klib.h>
#define MAX_SIZE 100

typedef struct {
    char* data[MAX_SIZE];
    int front;
    int rear;
} Queue;

void initQueue(Queue* queue);
int isQueueEmpty(Queue* queue);
int isQueueFull(Queue* queue);
void enqueue(Queue* queue, char* element);
void dequeue(Queue* queue);
char* front(Queue* queue);
int findElement(Queue* queue, char* name);
#endif