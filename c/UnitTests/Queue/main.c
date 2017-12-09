/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS Add-ons project.
 *
 *  Source Code:
 *  https://github.com/michaelbecker/freertos-addons
 *
 *  Project Page:
 *  http://michaelbecker.github.io/freertos-addons/
 *
 *  On-line Documentation:
 *  http://michaelbecker.github.io/freertos-addons/docs/html/index.html
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so,subject to the
 *  following conditions:
 *
 *  + The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *  + Credit is appreciated, but not required, if you find this project
 *    useful enough to include in your application, product, device, etc.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "queue_simple.h"


/**
 *  Deliberately insert the Node link in the middle of the struct.
 *  This is worst case and tests that we can recover the data after 
 *  dealing with the Nodes.
 */
typedef struct TestDataNode_t_ {

    int Data1;
    DlNode_t Node;
    int Data2;

} TestDataNode_t;


/*
 *  Helper function to allocate and init test nodes.
 */
TestDataNode_t *CreateTestDataNode(int d1, int d2)
{
    TestDataNode_t *n;

    n = malloc(sizeof(TestDataNode_t));

    if (!n)
        return NULL;

    n->Node.Next = NULL;
    n->Node.Prev = NULL;
    n->Data1 = d1;
    n->Data2 = d2;

    return n;
}


/*
 *  Helper function.
 *
 *  We cheat and use the internal DList to iterate the Queue.
 *
 *  @note! The internal structure of the queue actually stores items
 *  in reverse order. This isn't a problem, unless you cheat (like 
 *  we are) and peek inside. In this case, just use the reverse 
 *  iterator.
 */
void PrintQueue(const char *Title, Queue_t *Queue)
{
    TestDataNode_t *Node;
    DlNode_t *Cur;

    printf("\n%s\n", Title);
    printf("---------------------\n");
    printf("Count: %d\n", Queue->Count);
    DlForEachNodeReverse(&Queue->Head, Cur) {
        Node = CONTAINING_RECORD(Cur, TestDataNode_t, Node);
        printf("DATA: (%d, %d)\n", Node->Data1, Node->Data2);
    }
}


void FreeQueue(Queue_t *Queue)
{
    TestDataNode_t *Node;
    DlNode_t *Cur;

    while(!IsQueueEmpty(Queue)) {
        Cur = Dequeue(Queue);
        Node = CONTAINING_RECORD(Cur, TestDataNode_t, Node);
        free(Node);
    }
}


void FreeTestDataNode(DlNode_t *Node)
{
    TestDataNode_t *TestDataNode;

    TestDataNode = CONTAINING_RECORD(Node, TestDataNode_t, Node);
    free(TestDataNode);
}


int main (void)
{
    Queue_t Queue;
    TestDataNode_t *n[5];    
    int i;
    DlNode_t *Node1;
    DlNode_t *Node2;

    /* -------------------------------- */
    InitQueue(&Queue);
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        Enqueue(&Queue, &n[i]->Node);
    }

    PrintQueue("Test 1", &Queue);

    FreeQueue(&Queue);

    /* -------------------------------- */
    InitQueue(&Queue);
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        Enqueue(&Queue, &n[i]->Node);
    }

    Node1 = Dequeue(&Queue); /* n1 */
    Node2 = Dequeue(&Queue); /* n2 */
    FreeTestDataNode(Node2);

    Enqueue(&Queue, Node1);

    PrintQueue("Test 2", &Queue);

    FreeQueue(&Queue);

    /* -------------------------------- */
    InitQueue(&Queue);
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        Enqueue(&Queue, &n[i]->Node);
    }

    for (i = 0; i < 6; i++) {
        Node1 = Dequeue(&Queue);
        if (Node1)
            FreeTestDataNode(Node1);
    }

    PrintQueue("Test 3", &Queue);

    FreeQueue(&Queue);
    
    return 0;
}


