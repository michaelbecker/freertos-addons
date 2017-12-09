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


#ifndef DLIST_H_
#define DLIST_H_


/**
 *  The doubly linked list structure.
 *
 *  This is designed to be embedded within your data 
 *  structure(s).
 *
 *  These lists require more storage overhead than a singly linked list 
 *  (two pointers per item), but almost all operations take O(1) time.
 */
typedef struct DlNode_t_ {

    /**
     *  Pointer to the next item in the list.
     */
    struct DlNode_t_ *Next;

    /**
     *  Pointer to the previous item in the list.
     */
    struct DlNode_t_ *Prev;

} DlNode_t;


/**
 *  Macro to initialize a list head.
 *
 *  @param _head Pointer to the list head.
 */
#define DlInitHead(_head)       \
{                               \
    (_head)->Next = (_head);    \
    (_head)->Prev = (_head);    \
}


/**
 *  Add a node to the list head.
 *  Runs in O(1) time.
 *
 *  @param Head A pointer to the existing list head.
 *  @param Node A pointer to the node you are adding.
 */
void DlAddNodeToHead(   DlNode_t *Head, 
                        DlNode_t *Node);


/**
 *  Add a node to the list tail.
 *  Runs in O(1) time.
 *  
 *  @param Head A pointer to the existing list head.
 *  @param Node A pointer to the node you are adding.
 */
void DlAddNodeToTail(   DlNode_t *Head,
                        DlNode_t *Node);


/**
 *  Removes the node from the list head.
 *  Runs in O(1) time.
 *
 *  @param Head A pointer to the existing list head.
 *  @return The node removed, or NULL for an empty list.
 */
DlNode_t *DlRemoveNodeFromHead(DlNode_t *Head);


/**
 *  Removes the node from the list tail.
 *  Runs in O(1) time.
 *  
 *  @param Head A pointer to the existing list head.
 *  @return The node removed, or NULL for an empty list.
 */
DlNode_t *DlRemoveNodeFromTail(DlNode_t *Head);


/**
 *  Check if the list is empty.
 *
 *  @param _head A pointer to the existing list head.
 *  @return true if the list is empty, false otherwise.
 */
#define DlIsListEmpty(_head) \
    ((_head)->Next == _head)


/**
 *  Inserts a new node into the list right after the marker element.
 *  Runs in O(1) time.
 *
 *  @param Marker The node you are inserting after. Cannot be NULL.
 *  @param Node The node you are inserting. Cannot be NULL.
 */
void DlInsertNodeAfter( DlNode_t *Marker,
                        DlNode_t *Node);


/**
 *  Inserts a new node into the list right before the marker element.
 *  Runs in O(1) time.
 *
 *  @param Marker Node you are inserting before. Cannot be NULL.
 *  @param Node The node you are inserting. Cannot be NULL.
 */
void DlInsertNodeBefore(DlNode_t *Marker, 
                        DlNode_t *Node);


/**
 *  Removes a node from the list.
 *  Runs in O(1) time.
 *
 *  @param Node The node you are removing.
 */
void DlRemoveNode(DlNode_t *Node);


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
 *  Runs in O(n) time.
 *
 *  @param _head A pointer to the list head. Cannot be NULL.
 *  @param _node An DlNode_t pointer that you need to define before calling
 *                  this macro.
 */
#define DlForEachNode(_head, _node) \
    for ((_node) = (_head)->Next; (_node) != (_head); (_node) = (_node)->Next)

/**
 *  Macro to ease walking through all of the nodes in a list.
 *  Runs in O(n) time.
 *
 *  @param _head A pointer to the list head. Cannot be NULL.
 *  @param _node An DlNode_t pointer that you need to define before calling
 *                  this macro.
 */
#define DlForEachNodeReverse(_head, _node) \
    for ((_node) = (_head)->Prev; (_node) != (_head); (_node) = (_node)->Prev)

#endif


