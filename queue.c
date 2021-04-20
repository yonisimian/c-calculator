#include "queue.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"

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
    int longest_node; // the length of the longest node's data
} *Queue;

/** Create a node whose data is in the next format:
 * "<expression> = <result>" */
static qNode* qNodeCreate(char* expression, double result);

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

void queuePrint(Queue queue)
{
    if (queue == NULL || queue->size == 0)
    {
        char result[7 * 22];
        char lines[1][MAX_LENGTH];
        strcpy(lines[0], "No history.\n");
        makeItCool(result, lines, 1, 12);
        printf("%s\n", result);
        return;
    }

    int num_of_lines = queue->size;
    int max_length = queue->longest_node + 4;

    char result[(num_of_lines + 6) * (max_length + 10)];
    char lines[num_of_lines][MAX_LENGTH];
    int i = num_of_lines;
    for (qNode* ptr = queue->front; ptr != NULL; ptr = ptr->next, i--)
        sprintf(lines[num_of_lines - i], "%d. %s\n", i, ptr->string);
    
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
    node->next = NULL;

    return node;
}