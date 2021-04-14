#ifndef QUEUE_H_
#define QUEUE_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
static qNode* qNodeCreate(char* expression, double result);

// Creates a new empty queue
Queue queueCreate(void)
{
    Queue queue = (Queue)malloc(sizeof(Queue));
    if (queue == NULL)
        return NULL;
    
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    queue->longest_node = 0;

    return queue;
}

// Adds an item to the end of the queue.
// Returns the number of successfully added items.
int enqueue(Queue queue, char* expression, double result)
{
    if (queue == NULL)
        return 0;
    
    qNode* new_node = qNodeCreate(expression, result);
    if (new_node == NULL)
        return 0;

    if (queue->size == 0)
        queue->front = new_node;
    else
        queue->rear->next = new_node;

    queue->rear = new_node;
    queue->size++;
    int length = strlen(new_node->string);
    if (length > queue->longest_node)
        queue->longest_node = length;

    if (queue->size > MAX_HISTORY_COUNT)
        dequeue(queue);

    return 1;
}

// Pops the front of a queue
void dequeue(Queue queue)
{
    if (queue == NULL || queue->size == 0)
        return;
    
    qNode* front = queue->front;
    queue->front = queue->front->next;
    queue->size--;
    int length = strlen(front->string);
    free(front);

    // update longest node if needed
    if (length == queue->longest_node)
    {
        queue->longest_node = 0;
        qNode* ptr = queue->front;
        while (ptr != NULL)
        {
            length = strlen(ptr->string);
            if (length > queue->longest_node)
                queue->longest_node = length;
            ptr = ptr->next;
        }
    }

    if (queue->size == 0)
        queue->rear = NULL;
}

void queueDestroy(Queue queue)
{
    if (queue == NULL)
        return;
    
    if (queue->size == 0)
    {
        free(queue);
        return;
    }

    dequeue(queue);

    queueDestroy(queue);
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
    printf("*********************\n");
    printf("                     \n");
    printf("showing history:     \n");    
    if (queue == NULL || queue->size == 0)
        printf("No history.\n");
    else
    {
        int i = queue->size;
        for (qNode* ptr = queue->front; ptr != NULL; ptr = ptr->next)
            printf("%d. %s\n", i--, ptr->string);
    }
    printf("                     \n");
    printf("*********************\n");
}

void queuePrint2(Queue queue)
{
    if (queue == NULL || queue->size == 0)
    {
        printf("No history.\n");
        return;
    }

    int num_of_lines = queue->size;
    int max_length = queue->longest_node + 40;

    char result[(num_of_lines + 6) * (max_length + 10)];
    char lines[num_of_lines][max_length];
    int i = num_of_lines;
    for (qNode* ptr = queue->front; ptr != NULL; ptr = ptr->next, i--)
        sprintf(lines[num_of_lines - i], "%d. %s\n", i, ptr->string);
    printf("lines[1]: %s", lines[1]);
    
    makeItCool(result, lines, num_of_lines, max_length);
    printf("%s", result);
}

static qNode* qNodeCreate(char* expression, double result)
{
    qNode* node = (qNode*)malloc(sizeof(qNode));
    if (node == NULL)
        return NULL;

    char res[MAX_LENGTH];
    sprintf(res, "%g", result);

    int i;
    for (i = 0; (*(node->string + i) = *(expression + i)) != '\0'; i++);
    *(node->string + i++) = ' ';
    *(node->string + i++) = '=';
    *(node->string + i++) = ' ';
    for (int j = 0; (*(node->string + i) = *(res + j)) != '\0'; i++, j++);
    //node->expression = strcpy(node->expression, expression);
    node->next = NULL;

    return node;
}


#endif