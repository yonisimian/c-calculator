#ifndef QUEUE_H_
#define QUEUE_H_

#include "utils.h"

#define MAX_HISTORY_COUNT 50

typedef struct qNode
{
    char string[MAX_LENGTH * 2]; //more length for the result.
    struct qNode* next;
} qNode;

typedef struct Queue
{
    qNode* front;
    qNode* rear;
    int size;
    int longest_node;
} *Queue;

Queue queueCreate(void);
int enqueue(Queue queue, char* expression, double result);
void dequeue(Queue queue);
void queueDestroy(Queue queue);
int queueSize(Queue queue);
void queuePrint(Queue queue);
void queuePrint2(Queue queue);

#endif