/**
 *  @file
 *
 *  Singly linked list interface.
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
#ifndef SLIST_H_
#define SLIST_H_


/**
 *  The singly linked list structure.
 *  This is designed to be embedded within your data 
 *  structure(s).
 */
typedef struct SListNode_t_ {

    struct SListNode_t_ *Next;

} SListNode_t;


/**
 *  Macro to initialize a list head.
 *  @param _head The list head.
 */
#define InitSListHead(_head) \
    (_head)->Next = NULL;


/**
 *  Add a node to the list head.
 *  Runs in O(1) time.
 *
 *  @param Head Existing list head.
 *  @param Node The node you are adding to the head.
 */
#define AddNodeToHead(_head, _node) \
    InsertNodeAfter(_head, _node)


/**
 *  Add a node to the list tail.
 *  Runs in O(n) time.
 *  
 *  @param Head Existing list head.
 *  @param Node The node you are adding to the tail.
 */
void AddNodeToTail( SListNode_t *Head,
                    SListNode_t *Node);


/**
 *  Removes the node from the list head.
 *  Runs in O(1) time.
 *
 *  @param Head Existing list head.
 *  @return The node removed, or NULL for an empty list.
 */
SListNode_t *RemoveNodeFromHead(SListNode_t *Head);


/**
 *  Removes the node from the list tail.
 *  Runs in O(n) time.
 *  
 *  @param Head Existing list head.
 *  @return The node removed, or NULL for an empty list.
 */
SListNode_t *RemoveNodeFromTail(SListNode_t *Head);


/**
 *  Check if the list is empty.
 *
 *  @return true if the list is empty, false otherwise.
 */
#define IsSListEmpty(_head) \
    ((_head)->Next == NULL)


/**
 *  Inserts a new node into the list right after the marker element.
 *  Runs in O(1) time.
 *
 *  @param marker Node you are inserting after. Cannot be NULL.
 *  @param n The node you are inserting. Cannot be NULL.
 */
void InsertNodeAfter(   SListNode_t *Marker,
                        SListNode_t *Node);


/**
 *  Inserts a new node into the list right before the marker element.
 *  Runs in O(n) time.
 *
 *  @param head Existing list head.
 *  @param marker Node you are inserting after. Cannot be NULL.
 *  @param n The node you are inserting. Cannot be NULL.
 */
void InsertNodeBefore(  SListNode_t *Head, 
                        SListNode_t *Marker, 
                        SListNode_t *Node);


/**
 *  Removes a node from the list.
 *  Runs in O(n) time.
 *
 *  @param head Existing list head.
 *  @param n The node you are removing.
 */
void RemoveNode(SListNode_t *Head, 
                SListNode_t *Node);


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
#define ForEachSListNode(_head, _node) \
    for (SListNode_t *_node = _head->Next; _node != NULL; _node = _node->Next)


#endif

