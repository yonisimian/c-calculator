#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct Queue *Queue;

/** Create a new empty queue, and return it's pointer. */
Queue queueCreate(void);

/** Add an item (expression) to the end of the queue.
 *  Returns the number of successfully added items. */
int enqueue(Queue queue, char* expression, double result);

/** Pop the front of a queue */
void dequeue(Queue queue);

/** Remove and free a queue and it's nodes. */
void queueDestroy(Queue queue);

/** Return the size of a queue.
 * NOTE: if the queue is NULL, 0 will be returned. */
int queueSize(Queue queue);

/** Prints a queue in the next format (but prettified):
 * 4. 5+8*9 = 77
 * 3. 1+1 = 2
 * 2. -5 = -5
 * 1. sin(-3/2pi) = 1
 **/
void queuePrint(Queue queue);

#endif