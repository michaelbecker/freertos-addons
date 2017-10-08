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
MemoryPool_t *CreateMemoryPool( int itemSize, 
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
int AddExtraMemoryToPool(   MemoryPool_t *pool, 
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
MemoryPool_t *CreateMemoryPoolStatic(   int ItemSize,
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
int AddExtraMemoryToPoolStatic( MemoryPool_t *pool, 
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
void *MemoryPoolAllocate(MemoryPool_t *pool);


/**
 *  Return a memory buffer to the pool.
 *
 *  @note This can block, and cannnot be used from ISR context.
 *  @note There is no check that the memory passed in is valid.
 *
 *  @param pool A handle to a MemoryPool.
 *  @param memory memory obtained from MemoryPoolAllocate().
 */
void MemoryPoolFree(MemoryPool_t *pool, void *memory);


#endif

