
#include <stdlib.h>
#include "slist.h"


void SlAddNodeToTail(   SlNode_t *Head, 
                        SlNode_t *Node)
{
    //---------------------------
    SlNode_t *Current;
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


void SlInsertNodeAfter( SlNode_t *Marker,
                        SlNode_t *Node)
{
    //---------------------------
    SlNode_t *Temp;
    //---------------------------

    if (Marker == NULL)
        return;

    if (Node == NULL)
        return;

    Temp = Marker->Next;
    Marker->Next = Node;
    Node->Next = Temp;
}


void SlInsertNodeBefore(SlNode_t *Head, 
                        SlNode_t *Marker, 
                        SlNode_t *Node)
{
    //---------------------------
    SlNode_t *Current;
    SlNode_t *Prior;
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


void SlRemoveNode ( SlNode_t *Head, 
                    SlNode_t *Node)
{
    //---------------------------
    SlNode_t *Current;
    SlNode_t *Prior;
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


SlNode_t *SlRemoveNodeFromHead(SlNode_t *Head)
{
    //---------------------------
    SlNode_t *Node;
    //---------------------------
    
    if (Head == NULL)
        return;

    Node = Head->Next;

    if (Node != NULL) {
        Head->Next = Node->Next;
    }

    return Node;
}


SlNode_t *SlRemoveNodeFromTail(SlNode_t *Head)
{
    //---------------------------
    SlNode_t *Current;
    SlNode_t *Prior;
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


