/**
 *  @file
 *
 *  Singly linked list implementation.
 *
 *  These algorithms require a sentinal "Head" structure. This head is the 
 *  same as any element. As such,there is no room for optimizing tail 
 *  accesses so these will take O(n) time by default. Also, this implementation
 *  expects the list node to be embedded within your data, instead of wrapping 
 *  your data in it.
 *
 *  This requires C99 or higher.
 *
 *  The naming convention follows Pascal / Windows style.
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
#include <stdlib.h>
#include "slist.h"


void AddNodeToTail (    SListNode_t *Head, 
                        SListNode_t *Node)
{
    //---------------------------
    SListNode_t *Current;
    //---------------------------

    if (Head == NULL)
        return;

    if (Node == NULL)
        return;

    Current = Head;

    while (Current->Next != NULL){
        Current = Current->Next;
    }

    Current->Next = Node;
    Node->Next = NULL;
}


void InsertNodeAfter (  SListNode_t *Marker,
                        SListNode_t *Node)
{
    //---------------------------
    SListNode_t *Temp;
    //---------------------------

    if (Marker == NULL)
        return;

    if (Node == NULL)
        return;

    Temp = Marker->Next;
    Marker->Next = Node;
    Node->Next = Temp;
}


void InsertNodeBefore ( SListNode_t *Head, 
                        SListNode_t *Marker, 
                        SListNode_t *Node)
{
    //---------------------------
    SListNode_t *Current;
    SListNode_t *Prior;
    int Found = 0;
    //---------------------------

    if (Marker == NULL)
        return;

    if (Node == NULL)
        return;

    if (Head == NULL)
        return;

    Current = Head->Next;
    Prior = Head;

    while (Current != NULL){

        if (Current == Marker){
            Found = 1;
            break;
        }

        Prior = Current;
        Current = Current->Next;
    }

    if (Found) {
        Prior->Next = Node;
        Node->Next = Current;
    }
}


void RemoveNode (   SListNode_t *Head, 
                    SListNode_t *Node)
{
    //---------------------------
    SListNode_t *Current;
    SListNode_t *Prior;
    int Found = 0;
    //---------------------------

    if (Head == NULL)
        return;

    if (Node == NULL)
        return;

    Current = Head->Next;
    Prior = Head;

    while (Current != NULL){

        if (Current == Node){
            Found = 1;
            break;
        }

        Prior = Current;
        Current = Current->Next;
    }

    if (Found) {
        Prior->Next = Current->Next;
    }
}


SListNode_t *RemoveNodeFromHead(SListNode_t *Head)
{
    //---------------------------
    SListNode_t *Node;
    //---------------------------
    
    if (Head == NULL)
        return;

    Node = Head->Next;

    if (Node != NULL) {
        Head->Next = Node->Next;
    }

    return Node;
}


SListNode_t *RemoveNodeFromTail(SListNode_t *Head)
{
    //---------------------------
    SListNode_t *Current;
    SListNode_t *Prior;
    //---------------------------

    if (Head == NULL)
        return;

    Current = Head->Next;
    Prior = Head;

    if (IsSListEmpty(Head)) {
        return NULL;
    }

    while (Current->Next != NULL){

        Prior = Current;
        Current = Current->Next;
    }

    Prior->Next = NULL;

    return Current;
}


