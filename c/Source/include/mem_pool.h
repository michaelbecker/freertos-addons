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


#ifndef MEM_POOL_H_
#define MEM_POOL_H_


/**
 *  Handle for memory pools. 
 *
 *  These are fixed allocation size memory areas.
 */
typedef void * MemoryPool_t;


/**
 *  Create a MemoryPool
 *
 *  @param itemSize How big is an allocation.
 *  @param itemCount What's the maximum number of allocations allowed?
 *  @param Alignment Power of 2 value denoting on which address boundary the 
 *  memory will be aligned to. Must be at least sizeof(unsigned char *).
 *  @return A Handle to the pool, or NULL on failure.
 */
MemoryPool_t CreateMemoryPool(  int itemSize, 
                                int itemCount,
                                int Alignment);


/**
 *  There is no DeleteMemoryPool() by design!
 */


/**
 *  Allows you to add extra memory to a pool.
 *
 *  @param pool An existing memory pool.
 *  @param ItemCount How many more items you want to add.
 *  @return pdPASS on success, pdFAIL on error.
 */
int AddExtraMemoryToPool(   MemoryPool_t pool, 
                            int ItemCount);


/**
 *  Create a MemoryPool
 *
 *  @param ItemSize How big is an allocation.
 *  @param PreallocatedMemory Pointer to the preallocated memory
 *  you are dedicating to this pool.
 *  @param PreallocatedMemorySize How big is the buffer you are
 *  passing in.
 *  @param Alignment Power of 2 value denoting on which address boundary the 
 *  memory will be aligned to. Must be at least sizeof(unsigned char *).
 *  @return A Handle to the pool, or NULL on failure.
 */
MemoryPool_t CreateMemoryPoolStatic(int ItemSize,
                                    void *PreallocatedMemory,
                                    int PreallocatedMemorySize,
                                    int Alignment);


/**
 *  Allows you to add extra memory to a pool.
 *
 *  @param pool An existing memory pool.
 *  @param PreallocatedMemory Pointer to the preallocated memory
 *  you are dedicating to this pool.
 *  @param PreallocatedMemorySize How big is the buffer you are
 *  passing in.
 *  @return pdPASS on success, pdFAIL on error.
 */
int AddExtraMemoryToPoolStatic( MemoryPool_t pool, 
                                void *PreallocatedMemory,
                                int PreallocatedMemorySize);


/**
 *  Get a memory buffer from the pool.
 *
 *  Note that this can block, and cannnot be used from ISR context.
 *
 *  @param pool A handle to a MemoryPool.
 *  @return A pointer or NULL on failure.
 */
void *MemoryPoolAllocate(MemoryPool_t pool);


/**
 *  Return a memory buffer to the pool.
 *
 *  @note This can block, and cannnot be used from ISR context.
 *  @note There is no check that the memory passed in is valid.
 *
 *  @param pool A handle to a MemoryPool.
 *  @param memory memory obtained from MemoryPoolAllocate().
 */
void MemoryPoolFree(MemoryPool_t pool, void *memory);


#endif

