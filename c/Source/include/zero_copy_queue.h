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

