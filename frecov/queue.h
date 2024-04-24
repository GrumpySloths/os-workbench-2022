#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int front;
    int rear;
} Queue;

void initQueue(Queue* queue) {
    queue->front = -1;
    queue->rear = -1;
}

int isQueueEmpty(Queue* queue) {
    return (queue->front == -1 && queue->rear == -1);
}

int isQueueFull(Queue* queue) {
    return (queue->rear + 1) % MAX_SIZE == queue->front;
}

void enqueue(Queue* queue, int item) {
    if (isQueueFull(queue)) {
        printf("Queue is full. Cannot enqueue item.\n");
        return;
    }

    if (isQueueEmpty(queue)) {
        queue->front = 0;
        queue->rear = 0;
    } else {
        queue->rear = (queue->rear + 1) % MAX_SIZE;
    }

    queue->data[queue->rear] = item;
}

int dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue item.\n");
        return -1;
    }

    int item = queue->data[queue->front];

    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_SIZE;
    }

    return item;
}

#endif