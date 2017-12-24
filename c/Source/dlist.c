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


#include <stdlib.h>
#include "dlist.h"


void DlInsertNodeAfter( DlNode_t *Marker,
                        DlNode_t *Node)
{
    if (Marker == NULL)
        return;

    if (Node == NULL)
        return;

    Node->Next = Marker->Next;
    Node->Prev = Marker;

    Marker->Next->Prev = Node;
    Marker->Next = Node;
}


void DlInsertNodeBefore(DlNode_t *Marker,
                        DlNode_t *Node)
{
    DlInsertNodeAfter(Marker->Prev, Node);
}


void DlAddNodeToHead(   DlNode_t *Head, 
                        DlNode_t *Node)
{
    DlInsertNodeAfter(Head, Node);
}


void DlAddNodeToTail(   DlNode_t *Head,
                        DlNode_t *Node)
{
    DlInsertNodeAfter(Head->Prev, Node);
}


void DlRemoveNode(DlNode_t *Node)
{
    if (Node == NULL)
        return;

    Node->Next->Prev = Node->Prev;
    Node->Prev->Next = Node->Next;
}


DlNode_t *DlRemoveNodeFromHead(DlNode_t *Head)
{
    /****************/
    DlNode_t *Node;
    /****************/

    if (Head == NULL)
        return NULL;

    if (DlIsListEmpty(Head))
        return NULL;
    
    Node = Head->Next;

    DlRemoveNode(Node);

    return Node;
}


DlNode_t *DlRemoveNodeFromTail(DlNode_t *Head)
{
    /****************/
    DlNode_t *Node;
    /****************/

    if (Head == NULL)
        return NULL;

    if (DlIsListEmpty(Head))
        return NULL;

    Node = Head->Prev;

    DlRemoveNode(Node);

    return Node;
}


