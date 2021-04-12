#ifndef LIST_H_
#define LIST_H_

#include "utils.h"

typedef struct Node
{
    Op data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct List
{
    Node* head;
    int size;
} *List;

List listCreate(void);
int listAdd(List list, Op op);
void listCalculate(List list);
Op* listGet(List list, int index);
int listRemoveAt(List list, int index);
void listDestroy(List list);
int listSize(List list);
void listPrint(List list);
static void printOp(int index, Op* op);
static void calculateFunctions(List list);
static void calculateUnOp(List list);
static void calculateBinOp(List list, int round);
static void forceError(List list);
static Node* nodeRemove(List list, Node* node);

// Creates a new empty linked list
List listCreate(void)
{
    List list = (List)malloc(sizeof(List));
    if (list == NULL)
        return NULL;
    
    list->head = NULL;
    list->size = 0;

    return list;
}

// Adds an item to the end of the list.
// Returns the number of successfully added items.
int listAdd(List list, Op op)
{
    if (list == NULL)
        return 0;
    
    Node* ptr = list->head;
    while (ptr != NULL && ptr->next != NULL)
        ptr = ptr->next;
    
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL)
        return 0;

    new_node->data = op;
    new_node->prev = ptr;
    new_node->next = NULL;

    if (list->head == NULL)
        list->head = new_node;
    else
        ptr->next = new_node;

    list->size++;

    return 1;
}

// Calculates every operation in the list by the correct order of operations.
// Reduces the list so that the answer is in the first and only node.
// NOTE: if (list->size > 1) after the calculation, the expression was illegal. 
void listCalculate(List list)
{
    if (list == NULL || list->size == 0)
        return;

    calculateFunctions(list);   // reduces functions (sin, cos etc.)
    calculateUnOp(list);        // reduces factorials and minuses
    calculateBinOp(list, 1);    // reduces powers
    calculateBinOp(list, 2);    // recuces multiplications, divitions, and modulus.
    calculateBinOp(list, 3);    // reduces additions and substructions.
}

static void calculateFunctions(List list)
{
    Node* ptr = list->head;

    // go from right to left
    while(ptr->next != NULL)
        ptr = ptr->next;
    
    while(ptr != NULL)
    {
        Op op = ptr->data;

        if (op.type == FUNCTION)
        {
            // check for errors
            if (ptr->next == NULL || ptr->next->data.type != OPRAND)
            {
                    printf("SYNTAX ERROR: you must put an oprand after a function, my dude.\n");
                    forceError(list);
                    return;
            }

            switch((int)op.value)
            {
                case SIN:  ptr->data.value = sin(ptr->next->data.value); break;
                case COS:  ptr->data.value = cos(ptr->next->data.value); break;
                case TAN:  ptr->data.value = tan(ptr->next->data.value); break;
                case SQRT: ptr->data.value = sqrt(ptr->next->data.value); break;
                case LOG:  ptr->data.value = log10(ptr->next->data.value); break;
                case LN:   ptr->data.value = log(ptr->next->data.value); break;
                case ABS:  ptr->data.value = (ptr->next->data.value < 0) ? -(ptr->next->data.value) : ptr->next->data.value; break;           
                default:
                    printf("CODING ERROR: you shouldn't have reached here. My mistake :(\n");
                    forceError(list);
                    return;
            }
            ptr->data.type = OPRAND;
            nodeRemove(list, ptr->next);
        }

        ptr = ptr->prev;
    }
}

// calculates unary opertors: ! and -
// NOTE: - must be calculated here so 5^-1 will be legal expression.
static void calculateUnOp(List list)
{
    Node* ptr = list->head;
    while(ptr != NULL)
    {
        Op op = ptr->data;

        if (op.type == OPERATOR)
        {
            switch ((int)op.value)
            {
                case '!':
                {
                    ptr->prev->data.value = factorial(ptr->prev->data.value);
                    ptr = nodeRemove(list, ptr);
                    continue;
                }
                case '-':
                {
                    if (ptr->prev == NULL || ptr->prev->data.type != OPRAND)
                    {
                        if (ptr->next == NULL || ptr->next->data.type != OPRAND)
                        {
                            printf("SYNTAX ERROR: you must substract SOMETHING, fella.\n");
                            forceError(list);
                        }
                        else // prev node isn't OPRAND, next node is OPRAND
                        {
                            ptr->next->data.value *= -1;
                            ptr = nodeRemove(list, ptr);
                            continue;
                        }
                    }
                    break;
                }
                default:
                    ptr = ptr->next;
                    continue;
            }
        }
        else
            ptr = ptr->next;
    }
}

// calculates binary operators in the list.
// round 1 - calculates ^
// round 2 - calculates *, /, %
// round 3 - calculates +, -
static void calculateBinOp(List list, int round)
{
    Node* ptr = list->head;
    while(ptr != NULL)
    {
        Op op = ptr->data;

        if (op.type == OPERATOR)
        {
            switch (round)
            {
                case 1:
                    if ((int)op.value == '^')
                        ptr->prev->data.value = pow(ptr->prev->data.value, ptr->next->data.value);
                    else
                    {
                        ptr = ptr->next;
                        continue;
                    }
                    break;
                case 2:
                {
                    switch ((int)op.value)
                    {
                        case '*': ptr->prev->data.value *= ptr->next->data.value; break;
                        case '/': ptr->prev->data.value /= ptr->next->data.value; break;
                        case '%': ptr->prev->data.value = (int)(ptr->prev->data.value) % (int)(ptr->next->data.value); break;
                        default:  ptr = ptr->next; continue;
                    }
                    break;
                }
                case 3:
                {
                    switch ((int)op.value)
                    {
                        case '+': ptr->prev->data.value += ptr->next->data.value; break;
                        case '-': ptr->prev->data.value -= ptr->next->data.value; break;
                        default:  ptr = ptr->next; continue;
                    }
                    break;
                }
            }

            // we've reached here only if we encountered an operation. 
            Node* next = ptr->next->next;
            nodeRemove(list, ptr->next);
            nodeRemove(list, ptr);
            ptr = next; 
        }
        else // not an operator, skip
            ptr = ptr->next;
    }
}

// Returns the Op of the Node at a specific index 
Op* listGet(List list, int index)
{
    if (list == NULL || index < 0 || index >= list->size)
        return NULL;
    
    Node* ptr = list->head;
    int i = 0;
    while (ptr != NULL && ptr->next != NULL && i++ < index)
        ptr = ptr->next;

    return &ptr->data;
}

// Removes an item at a specific index in the list.
// Returns the number of successfully removed items.
int listRemoveAt(List list, int index)
{
    if (list == NULL || index < 0 || index >= list->size)
        return 0;
    
    Node* ptr = list->head;
    int i = 0;
    while (ptr != NULL && ptr->next != NULL && i++ < index)
        ptr = ptr->next;
    
    if (ptr->next != NULL)
    {
        if (ptr->prev != NULL) // ptr = mid element
        {
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;
        }
        else // ptr = first element
            ptr->next->prev = NULL;
    }
    else if (ptr->prev != NULL) // ptr = last element
            ptr->prev->next = NULL;

    free(ptr);
    list->size--;

    return 1;
}

void listDestroy(List list)
{
    Node* ptr = list->head;

    while (ptr != NULL)
    {
        Node* next = ptr->next;
        free(ptr);
        ptr = next;
    }

    free(list);
}

int listSize(List list)
{
    if (list == NULL)
        return 0;

    return list->size;
}

// Prints a list in the next format:
// [1, *, 3, +, 69, 420, !]
void listPrint(List list)
{
    if (list == NULL)
    {
        printf("NULL list\n");
        return;
    }
    
    if (list->head == NULL)
    {
        if (list->size != 0)
            printf("CODING ERROR: Something's wrong, I can feel it. list's size is %d but head is NULL.\n", list->size);

        printf("[]\n");
            return;
    }

    printf("[");
    Node* ptr = list->head;
    while (ptr != NULL)
    {
        Op cur_op = ptr->data;
        if (cur_op.type == OPERATOR)
            printf("%c, ", (int)cur_op.value);
        else
            printf("%g, ", cur_op.value);

        ptr = ptr->next;
    }
    printf("\b\b]\n");
}

// Prints a list in the next format:
// List of size 5:
// Op 1 - OPRAND - 1
// Op 2 - OPERATOR - +
// Op 3 - OPRAND - 69
// Op 4 - OPERATOR - *
// Op 5 - OPRAND - 420
// End of list.
void listPrint2(List list)
{
    if (list == NULL)
    {
        printf("NULL list\n");
        return;
    }
    
    if (list->head == NULL)
    {
        if (list->size != 0)
            printf("CODING ERROR: Something's wrong, I can feel it. list's size is %d but head is NULL.\n", list->size);

        printf("[]\n");
            return;
    }

    printf("List of size %d:\n", list->size);
    Node* ptr = list->head;
    for (int i = 0; ptr != NULL; i++, ptr = ptr->next)
        printOp(i, &ptr->data);
    
    printf("End of list.\n");
}

// Prints node's values in the next format:
// Op 1 - type OPRAND, value 5.5
static void printOp(int index, Op* op)
{
    char types[3][10] = { "UNKNOWN", "OPRAND", "OPERATOR" };
    if (op->type == OPERATOR)
        printf("Op %d - type %s, value %c\n", index, types[op->type], (int)(op->value));
    else
        printf("Op %d - type %s, value %g\n", index, types[op->type], op->value);
}

// Forces an error (to be used after an error has been found).
// takes any list and convert it to list one node with value INFINITY
static void forceError(List list)
{
    listDestroy(list);
    list = listCreate();
    listAdd(list, (Op){ .type = OPRAND, .value = INFINITY });
}

// C'mon tis not need for eggsplenashen
// okOK, it removes a node and connect the prev to the next.
// Oh, and it returns a pointer to the next node.
// NOTE: I'm assuming that node is in this specific list!
static Node* nodeRemove(List list, Node* node)
{
    if (node == NULL)
        return NULL;

    Node* prev = node->prev;
    Node* next = node->next;
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;

    if (list != NULL)
    {
        list->size--;
        if (node == list->head)
            list->head = next;
    }
    free(node);

    return next;
}

#endif