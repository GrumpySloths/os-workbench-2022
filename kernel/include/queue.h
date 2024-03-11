#ifndef QUEUE_H__
#define QUEUE_H__
#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#endif
#define MAX_SIZE 100

typedef struct {
    char* data[MAX_SIZE];
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

void enqueue(Queue* queue, char* element) {
    if (isQueueFull(queue)) {
        printf("Queue is full. Cannot enqueue element.\n");
        return;
    }

    if (isQueueEmpty(queue)) {
        queue->front = 0;
        queue->rear = 0;
    } else {
        queue->rear = (queue->rear + 1) % MAX_SIZE;
    }

    queue->data[queue->rear] = element;
}

void dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("Queue is empty. Cannot dequeue element.\n");
        return;
    }

    if (queue->front == queue->rear) {
        queue->front = -1;
        queue->rear = -1;
    } else {
        queue->front = (queue->front + 1) % MAX_SIZE;
    }
}

char* front(Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("Queue is empty. Cannot get front element.\n");
        return NULL;
    }

    return queue->data[queue->front];
}
//实现队列查询功能，给定一个char*name,返回队列中第一个匹配的元素的下标，如果没有匹配的元素，返回-1
int findElement(Queue* queue, char* name) {
    if (isQueueEmpty(queue)) {
        printf("Queue is empty. Cannot find element.\n");
        return -1;
    }

    int index = queue->front;
    while (index != queue->rear) {
        if (strcmp(queue->data[index], name) == 0) {
            return index;
        }
        index = (index + 1) % MAX_SIZE;
    }

    if (strcmp(queue->data[index], name) == 0) {
        return index;
    }

    return -1;
}

#endif