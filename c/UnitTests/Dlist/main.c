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
#include "dlist.h"


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


/**
 *  Helper function to clean up.
 *  @note You cannot use the DlForEachNode() macro here,
 *  because we are freeing each element. This would create a condition
 *  where we were accessing memory that we had just freed if we tried
 *  to use the macro.
 */
void FreeList(DlNode_t *Head)
{
    DlNode_t *Cur;
    TestDataNode_t *Node;

    while (!DlIsListEmpty(Head)) {
        Cur = DlRemoveNodeFromTail(Head);
        Node = CONTAINING_RECORD(Cur, TestDataNode_t, Node);
        free(Node);
    }
}


/*
 *  Helper function.
 *  @note This is a good place to use the ForEachSingleListNode()
 *  macro.
 */
void PrintList(const char *Title, DlNode_t *Head)
{
    TestDataNode_t *Node;
    DlNode_t *Cur;

    printf("\n%s\n", Title);
    printf("---------------------\n");

    DlForEachNode(Head, Cur) {
        Node = CONTAINING_RECORD(Cur, TestDataNode_t, Node);
        printf("DATA: (%d, %d)\n", Node->Data1, Node->Data2);
    } 
}


void TestMacros(void)
{
    TestDataNode_t T;

    printf("Test Macros\n");
    printf("---------------------\n");
    printf("OFFSET_OF(Data1) = %d\n", (int)OFFSET_OF(TestDataNode_t, Data1));
    printf("OFFSET_OF(Node)  = %d\n", (int)OFFSET_OF(TestDataNode_t, Node));
    printf("OFFSET_OF(Data2) = %d\n", (int)OFFSET_OF(TestDataNode_t, Data2));
    
    printf("&T       = %p\n", (void *)&T);
    printf("&T.Data1 = %p\n", (void *)&T.Data1);
    printf("&T.Node  = %p\n", (void *)&T.Node);
    printf("&T.Data2 = %p\n", (void *)&T.Data2);

    printf("CONTAINING_RECORD(Data1) = %p\n", (void *)CONTAINING_RECORD(&T.Data1, TestDataNode_t, Data1));
    printf("CONTAINING_RECORD(Node)  = %p\n", (void *)CONTAINING_RECORD(&T.Node,  TestDataNode_t, Node));
    printf("CONTAINING_RECORD(Data2) = %p\n", (void *)CONTAINING_RECORD(&T.Data2, TestDataNode_t, Data2));
}


int main (void)
{
    DlNode_t Head;
    TestDataNode_t *n[5];
    int i;
    DlNode_t *Node;
    TestDataNode_t *TestDataNode;


    /* -------------------------------- */
    TestMacros();

    /* -------------------------------- */
    DlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    DlAddNodeToHead(&Head, &n[1]->Node);
    DlAddNodeToHead(&Head, &n[0]->Node);

    DlAddNodeToTail(&Head, &n[3]->Node);
    DlAddNodeToTail(&Head, &n[4]->Node);

    DlInsertNodeAfter(&n[1]->Node, &n[2]->Node);

    PrintList("Test 1", &Head);
    FreeList(&Head);


    /* Test 2 -------------------------------- */
    DlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    for (i = 0; i < 5; i++)
        DlAddNodeToHead(&Head, &n[i]->Node);

    PrintList("Test 2", &Head);
    FreeList(&Head);


    /* Test 3 -------------------------------- */
    DlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    for (i = 0; i < 5; i++)
        DlAddNodeToTail(&Head, &n[i]->Node);

    PrintList("Test 3", &Head);
    FreeList(&Head);


    /* Test 4 -------------------------------- */
    DlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    DlInsertNodeAfter(&Head, &n[0]->Node);
    for (i = 0; i < (5-1); i++)
        DlInsertNodeAfter(&n[i]->Node, &n[i+1]->Node);

    PrintList("Test 4", &Head);
    FreeList(&Head);


    /* Test 5 -------------------------------- */
    DlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    DlAddNodeToHead(&Head, &n[0]->Node);
    for (i = 0; i < (5-1); i++)
        DlInsertNodeBefore(&n[i]->Node, &n[i+1]->Node);

    PrintList("Test 5", &Head);
    FreeList(&Head);



    DlInitHead(&Head);

    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        DlAddNodeToTail(&Head, &n[i]->Node);
    }

    PrintList("Start for Tests 6-10", &Head);


    /* Test 6 -------------------------------- */
    DlRemoveNode(&n[0]->Node);
    PrintList("Test 6", &Head);
    free(n[0]);

    /* Test 7 -------------------------------- */
    DlRemoveNode(&n[4]->Node);
    PrintList("Test 7", &Head);
    free(n[4]);

    /* Test 8 -------------------------------- */
    DlRemoveNode(&n[2]->Node);
    PrintList("Test 8", &Head);
    free(n[2]);

    /* Test 9 -------------------------------- */
    DlRemoveNode(&n[1]->Node);
    PrintList("Test 9", &Head);
    free(n[1]);

    /* Test 10 -------------------------------- */
    DlRemoveNode(&n[3]->Node);
    PrintList("Test 10", &Head);
    free(n[3]);


    /***********************************************************/
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        DlAddNodeToTail(&Head, &n[i]->Node);
    }
    
    PrintList("Test 11", &Head);

    for (i = 0; i < 6; i++) {
        Node = DlRemoveNodeFromHead(&Head);
        if (Node) {
            TestDataNode = CONTAINING_RECORD(Node, TestDataNode_t, Node);
            printf("\n\nRemoved: (%d, %d)\n", TestDataNode->Data1, TestDataNode->Data2);
        }
        else {
            printf("\n\nRemoved: List was empty!\n");
        }
        if (i < 5)
            free(TestDataNode);
        PrintList("Test 11 - cont", &Head);
    }


    /***********************************************************/
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        DlAddNodeToTail(&Head, &n[i]->Node);
    }
    
    PrintList("Test 12", &Head);

    for (i = 0; i < 6; i++) {
        Node = DlRemoveNodeFromTail(&Head);
        if (Node) {
            TestDataNode = CONTAINING_RECORD(Node, TestDataNode_t, Node);
            printf("\n\nRemoved: (%d, %d)\n", TestDataNode->Data1, TestDataNode->Data2);
        }
        else {
            printf("\n\nRemoved: List was empty!\n");
        }
        if (i < 5)
            free(TestDataNode);
        PrintList("Test 12 - cont", &Head);
    }
    

    FreeList(&Head);

    return 0;
}


