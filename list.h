#ifndef LIST_H_
#define LIST_H_

#include "defines.h"

typedef struct List *List;

/** Create a new empty linked list, and return it's pointer. */
List listCreate(void);

/** Add an item (op) to the end of the list.
 *  Returns the number of successfully added items. */
int listAdd(List list, Op op);

/** Calculate every operation in the list by the correct order of operations.
 * Reduces the list so that the answer is in the first and only node.
 * NOTE: if a calculation error occured, the answer will be INFINITY.
 * NOTE: if (list's size > 1) after the calculation, the expression was illegal. */ 
void listCalculate(List list);

/** Return the Node's data (Op) at a specific index */
Op* listGet(List list, int index);

/** Remove an item at a specific index in the list.
 * Returns the number of successfully removed items. */
int listRemoveAt(List list, int index);

/** Remove and free a list and it's nodes. */
void listDestroy(List list);

/** Return the size of a list.
 * NOTE: if the list is NULL, 0 will be returned. */
int listSize(List list);

/** Print a list in the next format:
 * [1, *, 3, +, 69, 420, !]
 * NOTE: made for debugging purposes. */
void listPrint(const List list);

/** Print a list in the next format:
 * List of size 5:
 * Op 1 - OPERAND - 1
 * Op 2 - OPERATOR - +
 * Op 3 - OPERAND - 69
 * Op 4 - OPERATOR - *
 * Op 5 - OPERAND - 420
 * End of list.
 * NOTE: made for debugging purposes.
 **/
void listPrint2(List list);

#endif