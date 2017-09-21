/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C Add-ons project.
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
 *  The FreeRTOS C Add-ons project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The FreeRTOS C Add-ons project is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the FreeRTOS C Add-ons project.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  NOTE: A modification to the GPL is included to allow you to distribute a
 *  combined work that includes FreeRTOS C Add-ons project
 *  without being obliged to provide the source
 *  code for proprietary components.
 * 
 *  The FreeRTOS C Add-ons project GPL exception text follows:
 * 
 *  Any FreeRTOS C Add-ons *source code*, whether modified or in it's 
 *  original release form, or whether in whole or in part, can only be 
 *  distributed by you under the terms of the GNU General Public License plus
 *  this exception.  An independent module is a module which is not derived
 *  from or based on FreeRTOS C Add-ons project.
 * 
 *  Clause 1:
 * 
 *  Linking FreeRTOS C Add-ons project  with other modules is making a 
 *  combined work based on FreeRTOS C Add-ons project. Thus, the terms 
 *  and conditions of the GNU General Public License V2 cover the
 *  whole combination.
 * 
 *  As a special exception, the copyright holders of FreeRTOS C Add-ons
 *  project give you permission to link FreeRTOS C Add-ons project with 
 *  independent modules to produce a statically linked executable, regardless
 *  of the license terms of these independent modules, and to copy and 
 *  distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the 
 *  terms and conditions of the license of that module.  An independent 
 *  module is a module which is not derived from or based on FreeRTOS C 
 *  Add-ons project.
 * 
 *  Clause 2:
 * 
 *  FreeRTOS C Add-ons project may not be used for any competitive or 
 *  comparative purpose, including the publication of any form of run time 
 *  or compile time metric, without the express permission of the copyright 
 *  holder(s) (this is the norm within the industry and is intended to ensure
 *  information accuracy).
 *  
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "slist.h"


/**
 *  Deliberately insert the Node link in the middle of the struct.
 *  This is worst case and tests that we can recover the data after 
 *  dealing with the Nodes.
 */
typedef struct TestDataNode_t_ {

    int Data1;
    SlNode_t Node;
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
    n->Data1 = d1;
    n->Data2 = d2;

    return n;
}


/**
 *  Helper function to clean up.
 *  @note You cannot use the SlForEachNode() macro here,
 *  because we are freeing each element. This would create a condition
 *  where we were accessing memory that we had just freed if we tried
 *  to use the macro.
 */
void FreeList(SlNode_t *Head)
{
    SlNode_t *Cur;
    TestDataNode_t *Node;

    Cur = Head->Next;

    while (Cur != NULL){
        Node = CONTAINING_RECORD(Cur, TestDataNode_t, Node);
        Cur = Cur->Next;
        free(Node);
    }
}


/*
 *  Helper function.
 *  @note This is a good place to use the ForEachSingleListNode()
 *  macro.
 */
void PrintList(const char *Title, SlNode_t *Head)
{
    TestDataNode_t *Node;
    SlNode_t *Cur;

    printf("\n%s\n", Title);
    printf("---------------------\n");

    SlForEachNode(Head, Cur) {
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
    SlNode_t Head;
    TestDataNode_t *n[5];
    int i;
    SlNode_t *Node;
    TestDataNode_t *TestDataNode;


    /* -------------------------------- */
    TestMacros();

    /* -------------------------------- */
    SlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    SlAddNodeToHead(&Head, &n[1]->Node);
    SlAddNodeToHead(&Head, &n[0]->Node);

    SlAddNodeToTail(&Head, &n[3]->Node);
    SlAddNodeToTail(&Head, &n[4]->Node);

    SlInsertNodeAfter(&n[1]->Node, &n[2]->Node);

    PrintList("Test 1", &Head);
    FreeList(&Head);


    /* Test 2 -------------------------------- */
    SlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    for (i = 0; i < 5; i++)
        SlAddNodeToHead(&Head, &n[i]->Node);

    PrintList("Test 2", &Head);
    FreeList(&Head);


    /* Test 3 -------------------------------- */
    SlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    for (i = 0; i < 5; i++)
        SlAddNodeToTail(&Head, &n[i]->Node);

    PrintList("Test 3", &Head);
    FreeList(&Head);


    /* Test 4 -------------------------------- */
    SlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    SlInsertNodeAfter(&Head, &n[0]->Node);
    for (i = 0; i < (5-1); i++)
        SlInsertNodeAfter(&n[i]->Node, &n[i+1]->Node);

    PrintList("Test 4", &Head);
    FreeList(&Head);


    /* Test 5 -------------------------------- */
    SlInitHead(&Head);

    for (i = 0; i < 5; i++)
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));

    SlAddNodeToHead(&Head, &n[0]->Node);
    for (i = 0; i < (5-1); i++)
        SlInsertNodeBefore(&Head, &n[i]->Node, &n[i+1]->Node);

    PrintList("Test 5", &Head);
    FreeList(&Head);



    SlInitHead(&Head);

    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        SlAddNodeToTail(&Head, &n[i]->Node);
    }

    PrintList("Start for Tests 6-10", &Head);


    /* Test 6 -------------------------------- */
    SlRemoveNode(&Head, &n[0]->Node);
    PrintList("Test 6", &Head);
    free(n[0]);

    /* Test 7 -------------------------------- */
    SlRemoveNode(&Head, &n[4]->Node);
    PrintList("Test 7", &Head);
    free(n[4]);

    /* Test 8 -------------------------------- */
    SlRemoveNode(&Head, &n[2]->Node);
    PrintList("Test 8", &Head);
    free(n[2]);

    /* Test 9 -------------------------------- */
    SlRemoveNode(&Head, &n[1]->Node);
    PrintList("Test 9", &Head);
    free(n[1]);

    /* Test 10 -------------------------------- */
    SlRemoveNode(&Head, &n[3]->Node);
    PrintList("Test 10", &Head);
    free(n[3]);


    /***********************************************************/
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        SlAddNodeToTail(&Head, &n[i]->Node);
    }
    
    PrintList("Test 11", &Head);

    for (i = 0; i < 6; i++) {
        Node = SlRemoveNodeFromHead(&Head);
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
        SlAddNodeToTail(&Head, &n[i]->Node);
    }
    
    PrintList("Test 12", &Head);

    for (i = 0; i < 6; i++) {
        Node = SlRemoveNodeFromTail(&Head);
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


