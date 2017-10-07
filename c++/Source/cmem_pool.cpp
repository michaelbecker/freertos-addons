/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
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
 *  The FreeRTOS C++ Wrappers project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The FreeRTOS C++ Wrappers project is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the FreeRTOS C++ Wrappers project.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  NOTE: A modification to the GPL is included to allow you to distribute a
 *  combined work that includes FreeRTOS C++ Wrappers project
 *  without being obliged to provide the source
 *  code for proprietary components.
 *
 *  The FreeRTOS C++ Wrappers project GPL exception text follows:
 *
 *  Any FreeRTOS C++ Wrapper *source code*, whether modified or in it's
 *  original release form, or whether in whole or in part, can only be
 *  distributed by you under the terms of the GNU General Public License plus
 *  this exception.  An independent module is a module which is not derived
 *  from or based on FreeRTOS C++ Wrappers project.
 *
 *  Clause 1:
 *
 *  Linking FreeRTOS C++ Wrappers project  with other modules is making a
 *  combined work based on FreeRTOS C++ Wrappers project. Thus, the terms
 *  and conditions of the GNU General Public License V2 cover the
 *  whole combination.
 *
 *  As a special exception, the copyright holders of FreeRTOS C++ Wrappers
 *  project give you permission to link FreeRTOS C++ Wrappers project with
 *  independent modules to produce a statically linked executable, regardless
 *  of the license terms of these independent modules, and to copy and
 *  distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the
 *  terms and conditions of the license of that module.  An independent
 *  module is a module which is not derived from or based on FreeRTOS C++
 *  Wrappers project.
 *
 *  Clause 2:
 *
 *  FreeRTOS C++ Wrappers project may not be used for any competitive or
 *  comparative purpose, including the publication of any form of run time
 *  or compile time metric, without the express permission of the copyright
 *  holder(s) (this is the norm within the industry and is intended to ensure
 *  information accuracy).
 *
 ***************************************************************************/


#include <stdlib.h>
#include "mem_pool.hpp"


using namespace cpp_freertos;


void MemoryPool::CalculateValidAlignment()
{
    /**
     *  Guarantee that the alignment is the size of a pointer.
     */
    if (Alignment < (int)sizeof(unsigned char *)) {
        Alignment = (int)sizeof(unsigned char *);
    }

    int alignmentBit = 0x1;
    int i;

    for (i = 0; i < 31; i++) {
        if (Alignment == alignmentBit) {
            break;
        }
        alignmentBit <<= 1;
    }

    if (i >= 31) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw MemoryPoolBadAlignmentException();
#else
        configASSERT(!"MemoryPool Bad Alignment");
#endif
    }
}


void MemoryPool::CalculateItemSize()
{
    if (ItemSize <= Alignment) {

        ItemSize = Alignment;
    }
    else {

        int alignmentCount = ItemSize / Alignment;
        if (ItemSize % Alignment != 0) {
            alignmentCount++;
        }

        ItemSize = alignmentCount * Alignment;
    }
}


MemoryPool::MemoryPool( int itemSize,
                        int itemCount,
                        int alignment)
    : ItemSize(itemSize),
      Alignment(alignment)
{
    CalculateValidAlignment();

    CalculateItemSize();

    unsigned char *address = (unsigned char *)malloc(ItemSize * itemCount);

    if (address == NULL) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw MemoryPoolMallocException();
#else
        configASSERT(!"MemoryPool malloc Failed");
#endif
    }

    for (int i = 0; i < itemCount; i++) {
        FreeItems.push_back(address);
        address += ItemSize;
    }

    Lock = new MutexStandard();
}


MemoryPool::MemoryPool( int itemSize,
                        void *preallocatedMemory,
                        int preallocatedMemorySize,
                        int alignment)
    : ItemSize(itemSize),
      Alignment(alignment)
{
    CalculateValidAlignment();

    CalculateItemSize();

    unsigned char *address = (unsigned char *)preallocatedMemory;

    while (preallocatedMemorySize >= ItemSize) {

        FreeItems.push_back(address);
        address += ItemSize;
        preallocatedMemorySize -= ItemSize;
    }

    Lock = new MutexStandard();
}


void *MemoryPool::Allocate()
{
    LockGuard guard(*Lock);

    if (FreeItems.empty())
        return NULL;

    void *item = FreeItems.front();
    FreeItems.pop_front();

    return item;
}


void MemoryPool::Free(void *item)
{
    LockGuard guard(*Lock);
    FreeItems.push_back(item);
}


void MemoryPool::AddMemory(int itemCount)
{
    unsigned char *address = (unsigned char *)malloc(ItemSize * itemCount);

    if (address == NULL) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw MemoryPoolMallocException();
#else
        configASSERT(!"MemoryPool AddMemory Failed");
#endif
    }

    for (int i = 0; i < itemCount; i++) {

        LockGuard guard(*Lock);

        FreeItems.push_back(address);
        address += ItemSize;
    }
}


void MemoryPool::AddMemory( void *preallocatedMemory,
                            int preallocatedMemorySize)
{
    unsigned char *address = (unsigned char *)preallocatedMemory;

    while (preallocatedMemorySize >= ItemSize) {

        LockGuard guard(*Lock);

        FreeItems.push_back(address);
        address += ItemSize;
        preallocatedMemorySize -= ItemSize;
    }
}


