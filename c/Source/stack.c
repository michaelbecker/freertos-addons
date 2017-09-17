/**
 *  @file stack.c
 *
 *  Implementation of a generic stack in C.
 */
/*{{{
 *  MIT License
 *
 *  Copyright (c) 2015, Michael Becker
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
 }}} */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"


void InitStack(Stack_t *Stack)
{
    Stack->Count = 0;
    InitSListHead(Stack->Head);
}


void PushOnStack(   Stack_t *Stack, 
                    SListNode_t *Node)
{
    if (!Stack)
        return;

    if (!Node)
        return;

    AddNodeToHead(Stack->Head, Node);
    Stack->Count++;
}


SListNode_t *PopOffStack(Stack_t *Stack)
{
    SListNode_t *Node;    

    if (!Stack)
        return;

    if (Stack->Count == 0) {
        return NULL;
    }
    else {
        Stack->Count--;
    }

    Node = RemoveNodeFromHead(Stack->Head);
    
    return Node;
}


