
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
    DlNode_t *Node;

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
    DlNode_t *Node;

    if (Head == NULL)
        return NULL;

    if (DlIsListEmpty(Head))
        return NULL;

    Node = Head->Prev;

    DlRemoveNode(Node);

    return Node;
}


