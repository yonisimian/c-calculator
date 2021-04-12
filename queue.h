#ifndef QUEUE_H_
#define QUEUE_H_

#include "utils.h"

typedef struct qNode
{
    char* expression;
    struct qNode* next;
} qNode;

typedef struct Queue
{
    qNode* front;
    qNode* rear;
    int size;
} *Queue;

Queue queueCreate(void);
int queueAdd(Queue queue, char* expression);
char* queueGet(Queue queue, int index);
void queueDestroy(Queue queue);
int queueSize(Queue queue);
void queuePrint(Queue queue);

// Creates a new empty queue
Queue queueCreate(void)
{
    Queue queue = (Queue)malloc(sizeof(Queue));
    if (queue == NULL)
        return NULL;
    
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;

    return queue;
}

// Adds an item to the end of the queue.
// Returns the number of successfully added items.
int queueAdd(Queue queue, char* expression)
{
    if (queue == NULL)
        return 0;
    
    qNode* new_node = (qNode*)malloc(sizeof(qNode));
    if (new_node == NULL)
        return 0;

    new_node->expression = expression;

    if (queue->front == NULL)
        queue->front = new_node;
    /*else
        ptr->next = new_node;*/

    queue->size++;

    return 1;
}

// Returns the string of the queue at a specific index 
char* queueGet(Queue queue, int index)
{
    if (queue == NULL || index < 0 || index >= queue->size)
        return NULL;
    
    qNode* ptr = queue->front;
    int i = 0;
    while (ptr != NULL && ptr->next != NULL && i++ < index)
        ptr = ptr->next;

    return NULL;
}

void queueDestroy(Queue queue)
{
    qNode* ptr = queue->front;

    while (ptr != NULL)
    {
        qNode* next = ptr->next;
        free(ptr);
        ptr = next;
    }

    free(queue);
}

int queueSize(Queue queue)
{
    if (queue == NULL)
        return 0;

    return queue->size;
}

// Prints a queue in the next format:
// **********************
// Showing history:
// 4. 5+8*9
// 3. 1+1
// 2. -5
// 1. sin(-3/2pi)
// **********************
void queuePrint(Queue queue)
{
    if (queue == NULL)
    {
        printf("NULL queue\n");
        return;
    }
    
    if (queue->front == NULL)
    {
        if (queue->size != 0)
            printf("CODING ERROR: Something's wrong, I can feel it. queue's size is %d but front is NULL.\n", queue->size);

        printf("[]\n");
            return;
    }

    printf("Queue of size %d:\n", queue->size);
    qNode* ptr = queue->front;
    for (int i = 0; ptr != NULL; i++, ptr = ptr->next)
        printf("%s\n", ptr->expression);
    
    printf("End of queue.\n");
}

#endif