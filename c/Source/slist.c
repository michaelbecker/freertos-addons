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
#include <stdlib.h>
#include "slist.h"


void SlAddNodeToTail(   SlNode_t *Head, 
                        SlNode_t *Node)
{
    /********************/
    SlNode_t *Current;
    /********************/

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
    /********************/
    SlNode_t *Temp;
    /********************/

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
    /********************/
    SlNode_t *Current;
    SlNode_t *Prior;
    int Found = 0;
    /********************/

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
    /********************/
    SlNode_t *Current;
    SlNode_t *Prior;
    int Found = 0;
    /********************/

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
    /********************/
    SlNode_t *Node;
    /********************/
    
    if (Head == NULL)
        return NULL;

    Node = Head->Next;

    if (Node != NULL) {
        Head->Next = Node->Next;
    }

    return Node;
}


SlNode_t *SlRemoveNodeFromTail(SlNode_t *Head)
{
    /********************/
    SlNode_t *Current;
    SlNode_t *Prior;
    /********************/

    if (Head == NULL)
        return NULL;

    Current = Head->Next;
    Prior = Head;

    if (SlIsListEmpty(Head)) {
        return NULL;
    }

    while (Current->Next != NULL){

        Prior = Current;
        Current = Current->Next;
    }

    Prior->Next = NULL;

    return Current;
}


