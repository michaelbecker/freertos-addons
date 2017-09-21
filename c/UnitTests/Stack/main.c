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
#include "stack_simple.h"


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


/*
 *  Helper function.
 *  We cheat and use the internal SList to iterate the stack.
 */
void PrintStack(const char *Title, Stack_t *Stack)
{
    TestDataNode_t *Node;
    SlNode_t *Cur;

    printf("\n%s\n", Title);
    printf("---------------------\n");
    printf("Count: %d\n", Stack->Count);
    SlForEachNode(&Stack->Head, Cur) {
        Node = CONTAINING_RECORD(Cur, TestDataNode_t, Node);
        printf("DATA: (%d, %d)\n", Node->Data1, Node->Data2);
    } 
}


void FreeStack(Stack_t *Stack)
{
    TestDataNode_t *Node;
    SlNode_t *Cur;

    while(!IsStackEmpty(Stack)) {
        Cur = PopOffStack(Stack);
        Node = CONTAINING_RECORD(Cur, TestDataNode_t, Node);
        free(Node);
    }
}


void FreeTestDataNode(SlNode_t *Node)
{
    TestDataNode_t *TestDataNode;

    TestDataNode = CONTAINING_RECORD(Node, TestDataNode_t, Node);
    free(TestDataNode);
}


int main (void)
{
    Stack_t Stack;
    TestDataNode_t *n[5];    
    int i;
    SlNode_t *Node1;
    SlNode_t *Node2;

    /* -------------------------------- */
    InitStack(&Stack);
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        PushOnStack(&Stack, &n[i]->Node);
    }

    PrintStack("Test 1", &Stack);

    FreeStack(&Stack);

    /* -------------------------------- */
    InitStack(&Stack);
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        PushOnStack(&Stack, &n[i]->Node);
    }

    Node1 = PopOffStack(&Stack); /* n4 */
    Node2 = PopOffStack(&Stack); /* n3 */
    FreeTestDataNode(Node2);

    PushOnStack(&Stack, Node1);

    PrintStack("Test 2", &Stack);

    FreeStack(&Stack);

    /* -------------------------------- */
    InitStack(&Stack);
    for (i = 0; i < 5; i++) {
        n[i] = CreateTestDataNode(i + 1,  10 + (i + 1));
        PushOnStack(&Stack, &n[i]->Node);
    }

    for (i = 0; i < 6; i++) {
        Node1 = PopOffStack(&Stack);
        if (Node1)
            FreeTestDataNode(Node1);
    }

    PrintStack("Test 3", &Stack);

    FreeStack(&Stack);
    
    return 0;
}


