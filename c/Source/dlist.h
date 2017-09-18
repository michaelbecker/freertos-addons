#ifndef DLIST_H_
#define DLIST_H_


/**
 *  The doubly linked list structure.
 *
 *  This is designed to be embedded within your data 
 *  structure(s).
 */
typedef struct DlNode_t_ {

    struct DlNode_t_ *Next;
    struct DlNode_t_ *Prev;

} DlNode_t;


/**
 *  Macro to initialize a list head.
 *  @param _head The list head.
 */
#define DLInitHead(_head)       \
{                               \
    (_head)->Next = (_head);    \
    (_head)->Prev = (_head);    \
}


/**
 *  Add a node to the list head.
 *  Runs in O(1) time.
 *
 *  @param Head Existing list head.
 *  @param Node The node you are adding to the head.
 */
void DlAddNodeToHead(   DlNode_t *Head, 
                        DlNode_t *Node);


/**
 *  Add a node to the list tail.
 *  Runs in O(1) time.
 *  
 *  @param Head Existing list head.
 *  @param Node The node you are adding to the tail.
 */
void DlAddNodeToTail(   DlNode_t *Head,
                        DlNode_t *Node);


/**
 *  Removes the node from the list head.
 *  Runs in O(1) time.
 *
 *  @param Head Existing list head.
 *  @return The node removed, or NULL for an empty list.
 */
DlNode_t *DlRemoveNodeFromHead(DlNode_t *Head);


/**
 *  Removes the node from the list tail.
 *  Runs in O(1) time.
 *  
 *  @param Head Existing list head.
 *  @return The node removed, or NULL for an empty list.
 */
DlNode_t *DlRemoveNodeFromTail(DlNode_t *Head);


/**
 *  Check if the list is empty.
 *
 *  @return true if the list is empty, false otherwise.
 */
#define DlIsListEmpty(_head) \
    ((_head)->Next == _head)


/**
 *  Inserts a new node into the list right after the marker element.
 *  Runs in O(1) time.
 *
 *  @param marker Node you are inserting after. Cannot be NULL.
 *  @param n The node you are inserting. Cannot be NULL.
 */
void DlInsertNodeAfter( DlNode_t *Marker,
                        DlNode_t *Node);


/**
 *  Inserts a new node into the list right before the marker element.
 *  Runs in O(1) time.
 *
 *  @param head Existing list head.
 *  @param marker Node you are inserting after. Cannot be NULL.
 *  @param n The node you are inserting. Cannot be NULL.
 */
void DlInsertNodeBefore(DlNode_t *Head, 
                        DlNode_t *Marker, 
                        DlNode_t *Node);


/**
 *  Removes a node from the list.
 *  Runs in O(1) time.
 *
 *  @param head Existing list head.
 *  @param n The node you are removing.
 */
void DlRemoveNode(  DlNode_t *Head, 
                    DlNode_t *Node);


/**
 *  Given here in case you do not have an equivalent macro.
 *  @param _type The structure type.
 *  @param _field The name of the field you want the offset to.
 *  @returns The offset into _type where _field starts, in bytes.
 */
#ifndef OFFSET_OF
#define OFFSET_OF(_type, _field)    \
    ((size_t)&((_type *)0)->_field)
#endif


/**
 *  Given here in case you do not have an equivalent macro.
 *  @param _address The real address of the _field you have.
 *  @param _type The structure type.
 *  @param _field The name of the field you want the offset to.
 *  @returns A typed pointer to the structure containing the _field 
 *  at _address.
 */
#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(_address, _type, _field)  \
    ((_type *)((unsigned char *)(_address) - OFFSET_OF(_type, _field)))
#endif


/**
 *  Macro to ease walking through all of the nodes in a list.
 *  @param _head The List head, cannot be NULL.
 *  @param _node An undefined symbol that will be defined by the macro.
 */
#define DlForEachNode(_head, _node) \
    for (DlNode_t *_node = _head->Next; _node != _head; _node = _node->Next)

/**
 *  Macro to ease walking through all of the nodes in a list.
 *  @param _head The List head, cannot be NULL.
 *  @param _node An undefined symbol that will be defined by the macro.
 */
#define DlForEachNodeReverse(_head, _node) \
    for (DlNode_t *_node = _head->Prev; _node != _head; _node = _node->Prev)

#endif


