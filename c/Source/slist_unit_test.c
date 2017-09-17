/**
 *  @file
 *
 *  Unit test for Singly linked list.
 *
 *  This requires C99 or higher.
 *  
 *  To compile:
 *  gcc -std=c99 -Wall single_list_a.c single_list_unit_test_a.c -o single_list_unit_test_a
 *  
 *  To test:
 *  valgrind single_list_unit_test_a
 *
 *  MIT License
 *
 *  Copyright (c) 2015, Michael Becker (michael.f.becker@gmail.com)
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a 
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included 
 *  in all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT 
 *  OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR 
 *  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include "single_list_a.h"


/**
 *  Deliberately insert the Node link in the middle of the struct.
 *  This is worst case and tests that we can recover the data after 
 *  dealing with the Nodes.
 */
typedef struct TestDataNode_ {

    int Data1;
    SingleListNode Node;
    int Data2;

}TestDataNode;


/*
 *  Helper function to allocate and init test nodes.
 */
TestDataNode *CreateTestDataNode(int d1, int d2)
{
    TestDataNode *n;

    n = malloc(sizeof(TestDataNode));

    if (!n)
        return NULL;

    n->Node.Next = NULL;
    n->Data1 = d1;
    n->Data2 = d2;

    return n;
}


/**
 *  Helper function to clean up.
 *  @note You cannot use the ForEachSingleListNode() macro here,
 *  because we are freeing each element. This would create a condition
 *  where we were accessing memory that we had just freed if we tried
 *  to use the macro.
 */
void FreeList(SingleListNode *Head)
{
    SingleListNode *Cur;
    TestDataNode *Node;

    Cur = Head->Next;

    while (Cur != NULL){
        Node = CONTAINING_RECORD(Cur, TestDataNode, Node);
        Cur = Cur->Next;
        free(Node);
    }
}


/*
 *  Helper function.
 *  @note This is a good place to use the ForEachSingleListNode()
 *  macro.
 */
void PrintList(const char *Title, SingleListNode *Head)
{
    TestDataNode *Node;

    printf("\n%s\n", Title);

    ForEachSingleListNode(Head, Cur) {
        Node = CONTAINING_RECORD(Cur, TestDataNode, Node);
        printf("DATA: (%d, %d)\n", Node->Data1, Node->Data2);
    } 
}



void TestMacros(void)
{
    printf("OFFSET_OF(Data1) = %d\n", (int)OFFSET_OF(TestDataNode, Data1));
    printf("OFFSET_OF(Node)  = %d\n", (int)OFFSET_OF(TestDataNode, Node));
    printf("OFFSET_OF(Data2) = %d\n", (int)OFFSET_OF(TestDataNode, Data2));
    
    TestDataNode T;
    printf("&T       = %p\n", &T);
    printf("&T.Data1 = %p\n", &T.Data1);
    printf("&T.Node  = %p\n", &T.Node);
    printf("&T.Data2 = %p\n", &T.Data2);

    printf("CONTAINING_RECORD(Data1) = %p\n", CONTAINING_RECORD(&T.Data1, TestDataNode, Data1));
    printf("CONTAINING_RECORD(Node)  = %p\n", CONTAINING_RECORD(&T.Node,  TestDataNode, Node));
    printf("CONTAINING_RECORD(Data2) = %p\n", CONTAINING_RECORD(&T.Data2, TestDataNode, Data2));
}



int main (void)
{
    SingleListNode Head;
    TestDataNode *n[5];
    int i;

    TestMacros();

    // Test 1 --------------------------------
    InitSingleListHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i, i * 2 + 1);

    AddNodeToHead(&Head, &n[1]->Node);
    AddNodeToHead(&Head, &n[0]->Node);

    AddNodeToTail(&Head, &n[3]->Node);
    AddNodeToTail(&Head, &n[4]->Node);

    InsertNodeAfter(&n[1]->Node, &n[2]->Node);

    PrintList("Test 1", &Head);

    FreeList(&Head);


    // Test 2 --------------------------------
    InitSingleListHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i, i * 3 + 1);

    for (i = 0; i < 5; i++)
        AddNodeToHead(&Head, &n[i]->Node);
    PrintList("Test 2", &Head);
    FreeList(&Head);


    // Test 3 --------------------------------
    InitSingleListHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i, i * 10 + 1);

    for (i = 0; i < 5; i++)
        AddNodeToTail(&Head, &n[i]->Node);

    PrintList("Test 3", &Head);
    FreeList(&Head);


    // Test 4 --------------------------------
    InitSingleListHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i, i * 100 + 1);

    InsertNodeAfter(&Head, &n[0]->Node);
    for (i = 0; i < (5-1); i++)
        InsertNodeAfter(&n[i]->Node, &n[i+1]->Node);

    PrintList("Test 4", &Head);
    FreeList(&Head);


    // Test 5 --------------------------------
    InitSingleListHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i, i * 1000 + 1);

    AddNodeToHead(&Head, &n[0]->Node);
    for (i = 0; i < (5-1); i++)
        InsertNodeBefore(&Head, &n[i]->Node, &n[i+1]->Node);

    PrintList("Test 5", &Head);
    FreeList(&Head);



    InitSingleListHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i, i * 50 + 1);

    for (i = 0; i < 5; i++) {
        AddNodeToTail(&Head, &n[i]->Node);
    }

    PrintList("Start for Tests 6-8", &Head);


    // Test 6 --------------------------------
    RemoveNode(&Head, &n[0]->Node);
    PrintList("Test 6", &Head);
    free(n[0]);

    // Test 7 --------------------------------
    RemoveNode(&Head, &n[4]->Node);
    PrintList("Test 7", &Head);
    free(n[4]);

    // Test 8 --------------------------------
    RemoveNode(&Head, &n[2]->Node);
    PrintList("Test 8", &Head);
    free(n[2]);

    FreeList(&Head);

    return 0;
}


