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
void listPrint2(List list);
static void printOp(int index, Op* op);
static void calculateFunctions(List list);
static void calculateUnOp(List list);
static void calculateBinOp(List list, int round);
static void forceError(List list);
static Node* nodeRemove(List list, Node* node);

#endif