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


