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
 ***************************************************************************/
#ifndef ZERO_COPY_QUEUE_H_
#define ZERO_COPY_QUEUE_H_


/**
 *  Handle for ZeroCopyQueues. 
 *
 *  These queues only copy a single pointer instead of the 
 *  entire data structure, which is how FreeRTOS works by default.
 */
typedef void * ZeroCopyQueue_t;


/**
 *  Create a Zero Copy Queue.
 *
 *  @param itemSize Maximum size of the item.
 *  @param itemCount What's the maximum number of items allowed?
 *  @param Alignment Power of 2 value denoting on which address boundary the 
 *  memory will be aligned to. Must be at least sizeof(unsigned char *).
 *  @return A Handle to the ZeroCopyQueue, or NULL on failure.
 */
ZeroCopyQueue_t ZcqCreateQueue( int itemSize,
                                int itemCount,
                                int Alignment);


/**
 *  Allocate an item to use or queue.
 *
 *  Note that this can block, and cannnot be used from ISR context.
 *
 *  @param zcq A handle to a ZeroCopyQueue_t.
 *  @return A pointer or NULL on failure.
 */
void *ZcqAllocateItem(ZeroCopyQueue_t zcq);


/**
 *  Free a previously allocated item back into the base pool.
 *
 *  @note This can block, and cannnot be used from ISR context.
 *  @note There is no check that the memory passed in is valid.
 *
 *  @param zcq A handle to a ZeroCopyQueue_t.
 *  @param item An item obtained from ZcqAllocateItem().
 */
void ZcqFreeItem(ZeroCopyQueue_t zcq, void *item);


/**
 *  Queue an item obtained from ZcqAllocateItem().
 *
 *  @param zcq A handle to a ZeroCopyQueue_t.
 *  @param item An item obtained from ZcqAllocateItem().
 *  @param Timeout Timeout in FreeRTOS ticks.
 *  @return 1 on success, 0 on failure.
 */
int ZcqEnqueueItem(ZeroCopyQueue_t zcq, void *item, TickType_t Timeout);


/**
 *  Dequeue an item from ZcqEnqueueItem().
 *
 *  @param zcq A handle to a ZeroCopyQueue_t.
 *  @param Timeout Timeout in FreeRTOS ticks.
 *  @return An item obtained from ZcqAllocateItem() on success.
 *  NULL on failure.
 */
void *ZcqDequeueItem(ZeroCopyQueue_t zcq, TickType_t Timeout);


#endif

