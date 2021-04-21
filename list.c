#include "list.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"

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

static void calculateFunctions(List list);
static void calculateUnOp(List list);
static void calculateBinOp(List list, int round);
static void forceError(List list);
static void printOp(int index, Op* op);
static Node* nodeRemove(List list, Node* node);
static int factorial(int num);

List listCreate(void)
{
    List list = malloc(sizeof(*list));
    if (list == NULL)
        return NULL;
    
    list->head = NULL;
    list->size = 0;

    return list;
}

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

// ====================== listCalculate ====================== //
void listCalculate(List list)
{
    if (list == NULL || list->size == 0)
        return;

    calculateFunctions(list);   // reduces functions (sin, cos etc.)
    calculateUnOp(list);        // reduces factorials and minuses
    calculateBinOp(list, 1);    // reduces powers
    calculateBinOp(list, 2);    // recuces multiplications, divitions, and modulus.
    calculateBinOp(list, 3);    // reduces additions and substructions.

    // each expression should reduce to 1 node
    // so if there's more than 1 node, probably an unappropriate space was entered.
    if (list->size > 1)
    {
        printf("%s: more than 1 expression was entered (check for unappropriate spaces)\n", SYNTAX_ERROR);
        forceError(list);
        return;
    }
}

/** calculates functions: sin cos etc.
 * NOTE: calculates from right to left (of the expression) */
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
                printf("%s: you must put an oprand after a function, my dude.\n", SYNTAX_ERROR);
                forceError(list);
                return;
            }

            if (ptr->next->data.value <= 0 && (op.value == LOG || op.value == LN))
            {
                printf("%s: Remember! log and ln can only operate on positive values.\n", MATH_ERROR);
                forceError(list);
                return;
            }
            
            if (ptr->next->data.value < 0 && op.value == SQRT)
            {
                printf("%s: Remember! sqrt can only operate on non-negative values.\n", MATH_ERROR);
                forceError(list);
                return;
            }

            if ((ptr->next->data.value / (pi/2)) == 1 && op.value == TAN)
            {
                printf("%s: tan of (pi+2k)/2 is undefined.\n", MATH_ERROR);
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
                    printf("%s: you shouldn't have reached here. My mistake :|\n", CODING_ERROR);
                    forceError(list);
                    return;
            }
            ptr->data.type = OPRAND;
            nodeRemove(list, ptr->next);
        }

        ptr = ptr->prev;
    }
}

/** calculates unary opertors: ! and -
 * NOTE: (-) must be calculated here so 5^-1 will be a legal expression. */
static void calculateUnOp(List list)
{
    Node* ptr = list->head;
    while(ptr != NULL)
    {
        Op op = ptr->data;

        if (op.type == OPERATOR)
        {
            // checking for errors first
            if (ptr->next == NULL && op.value != '!') // basically if end with (-)
            {
                printf("%s: you can't end with a %c, sire!\n", SYNTAX_ERROR, (int)ptr->data.value);
                forceError(list);
                return;
            }

            // no fatal errors
            switch ((int)op.value)
            {
                case '!':
                {
                    Node* prev = ptr->prev;
                    // error checking
                    if (prev == NULL)
                    {
                        printf("%s: you are a pretty girl, but you must have an oprand before factorial :(\n", SYNTAX_ERROR);
                        forceError(list);
                        return;
                    }

                    if (prev->data.type != OPRAND)
                    {
                        printf("%s: ! works on oprands (numbers) only, babe ^^\n", SYNTAX_ERROR);
                        forceError(list);
                        return;
                    }

                    if (prev->data.value - (int)prev->data.value != 0)
                    {
                        printf("%s: ! works on integers only uwu\n", MATH_ERROR);
                        forceError(list);
                        return;
                    }

                    if (prev->data.value > 31)
                    {
                        printf("%s: the result was too dangerous to be left alive!\n", MATH_ERROR);
                        forceError(list);
                        return;
                    }

                    // no errors
                    if (prev->data.value < 0) // so -5! become -(5!)
                    {
                        prev->data.value = factorial(-prev->data.value) * -1;
                        ptr = nodeRemove(list, ptr);
                        continue;
                    }
                    else // normal calculation, like 5!
                    {
                        prev->data.value = factorial(prev->data.value);
                        ptr = nodeRemove(list, ptr);
                        continue;
                    }
                }
                case '-':
                {
                    if (ptr->next == NULL)
                    {
                        printf("%s: you can't end with a (-), sire!\n", SYNTAX_ERROR);
                        forceError(list);
                        return;
                    }
                    else if (ptr->prev == NULL || ptr->prev->data.type != OPRAND)
                    {
                        if (ptr->next->data.type != OPRAND)
                        {
                            printf("%s: you must substract SOMETHING, fella.\n", SYNTAX_ERROR);
                            forceError(list);
                            return;
                        }
                        else // prev node isn't OPRAND, next node is OPRAND
                        {
                            ptr->next->data.value *= -1;
                            ptr = nodeRemove(list, ptr);
                            continue;
                        }
                    }
                    else // if (next and prev aren't NULL) is taken care in round 3
                    {
                        ptr = ptr->next;
                        continue;
                    }
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

/** calculates binary operators in the list.
 * round 1 - calculates ^
 * round 2 - calculates *, /, %
 * round 3 - calculates +, -
 **/
static void calculateBinOp(List list, int round)
{
    Node* ptr = list->head;
    while(ptr != NULL)
    {
        Op op = ptr->data;

        if (op.type == OPERATOR)
        {
            // checking for errors first
            if (ptr->next == NULL || ptr->prev == NULL)
            {
                printf("%s: you can't begin or end with a %c, sire!\n", SYNTAX_ERROR, (int)ptr->data.value);
                forceError(list);
                return;
            }
            if (ptr->prev->data.type != OPRAND || ptr->next->data.type != OPRAND)
            {
                printf("%s: you must have oprands before and after your %c, honey ~:\n", SYNTAX_ERROR, (int)ptr->data.value);
                forceError(list);
                return;
            }

            // no errors
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
                        case '/':
                        {
                            // check for division by zero
                            if (ptr->next->data.value - 0 == 0)
                            {
                                printf("%s: you can't divide by zero, m'lord.\n", MATH_ERROR);
                                forceError(list);
                                return;
                            }
                            ptr->prev->data.value /= ptr->next->data.value;
                            break;
                        }
                        case '%':
                        {
                            // check for integers before and after
                            if (ptr->prev->data.value != (int)ptr->prev->data.value || ptr->next->data.value != (int)ptr->next->data.value
                                    || ptr->prev->data.value < 0 || ptr->next->data.value <= 0)
                            {
                                printf("%s: modulu (%%) works only on positive integers, m'lady.\n", MATH_ERROR);
                                forceError(list);
                                return;
                            }
                            ptr->prev->data.value = (int)(ptr->prev->data.value) % (int)(ptr->next->data.value);
                            break;
                        }
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
                default:
                    printf("%s: developer did something stupid, abort ABORT.\n", CODING_ERROR);
                    forceError(list);
                    return;
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
// ====================== end of listCalculate ====================== //
 
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
    if (list == NULL)
        return;
    
    if (list->head == NULL)
    {
        free(list);
        return;
    }

    Node* ptr = list->head->next;
    free(list->head);
    list->head = ptr;
    list->size--;

    listDestroy(list);
}

int listSize(List list)
{
    if (list == NULL)
        return 0;

    return list->size;
}

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
            printf("%s: Something's wrong, I can feel it. list's size is %d but head is NULL.\n", CODING_ERROR, list->size);

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
            printf("%s: Something's wrong, I can feel it. list's size is %d but head is NULL.\n", CODING_ERROR, list->size);

        printf("[]\n");
            return;
    }

    printf("List of size %d:\n", list->size);
    Node* ptr = list->head;
    for (int i = 0; ptr != NULL; i++, ptr = ptr->next)
        printOp(i, &ptr->data);
    
    printf("End of list.\n");
}

/** Print node's values in the next format:
 * Op 1 - type OPRAND, value 5.5 */
static void printOp(int index, Op* op)
{
    char types[3][10] = { "UNKNOWN", "OPRAND", "OPERATOR" };
    if (op->type == OPERATOR)
        printf("Op %d - type %s, value %c\n", index, types[op->type], (int)(op->value));
    else
        printf("Op %d - type %s, value %g\n", index, types[op->type], op->value);
}

/** Forces an error (to be used after an error has been found).
 * Takes any list and convert it to list one node with value ERROR_VALUE. */
static void forceError(List list)
{
    listDestroy(list);
    list = listCreate();
    listAdd(list, (Op){ .type = OPRAND, .value = ERROR_VALUE });
}

/** C'mon tis not need for eggsplenashen
 * okOK, it removes a node and connect the prev to the next.
 * Oh, and it returns a pointer to the next node.
 * NOTE: I'm assuming that node is in this specific list! */
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

/** Calculate the factorial of a non-negative number. */
static int factorial(int num)
{
    if (num < 0)
        return 0;
    
    if (num == 0 || num == 1)
        return 1;

    return num * factorial(num - 1);
}