/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
 *
 *  The FreeRTOS C++ Wrappers project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 3 of the License.
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
 ***************************************************************************/
#ifndef QUEUE_HPP_
#define QUEUE_HPP_


#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "queue.h"


namespace cpp_freertos {


/**
 *  This is the exception that is thrown if a Queue constructor fails.
 */
class QueueCreateException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        QueueCreateException()
        {
            sprintf(errorString, "Queue Constructor Failed");
        }

        /**
         *  Create the exception.
         */
        QueueCreateException(const char *info)
        {
            snprintf(errorString, sizeof(errorString),
                        "Queue Constructor Failed %s", info);
        }

        /**
         *  Get what happened as a string.
         *  We are overriding the base implementation here.
         */
        virtual const char *what() const throw()
        {
            return errorString;
        }

    private:
        /**
         *  A text string representing what failed.
         */
        char errorString[80];
};


/**
 *  Queue class wrapper for FreeRTOS queues. This class provides enqueue
 *  and dequeue operations.
 *
 *  @note It is expected that an application will instantiate this class or
 *        one of the derived classes and use that. It is not expected that
 *        a user or application will derive from these classes.
 */
class Queue {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Our constructor.
         *
         *  @throws QueueCreateException
         *  @param maxItems Maximum number of items thsi queue can hold.
         *  @param itemSize Size of an item in a queue.
         *  @note FreeRTOS queues use a memcpy / fixed size scheme for queues.
         */
        Queue(UBaseType_t maxItems, UBaseType_t itemSize);

        /**
         *  Our destructor.
         */
        virtual ~Queue();

        /**
         *  Add an item to the back of the queue.
         *
         *  @param item The item you are adding.
         *  @return true if the item was added, false if it was not.
         */
        virtual bool Enqueue(void *item);

        /**
         *  Add an item to the back of the queue.
         *
         *  @param item The item you are adding.
         *  @param Timeout How long to wait to add the item to the queue if
         *         the queue is currently full.
         *  @return true if the item was added, false if it was not.
         */
        virtual bool Enqueue(void *item, TickType_t Timeout);

        /**
         *  Remove an item from the front of the queue.
         *
         *  @param item Where the item you are removing will be returned to.
         *  @param Timeout How long to wait to remove an item if the queue
         *         is currently empty.
         *  @return true if an item was removed, false if no item was removed.
         */
        bool Dequeue(void *item, TickType_t Timeout = portMAX_DELAY);

        /**
         *  Make a copy of an item from the front of the queue. This will
         *  not remove it from the head of the queue.
         *
         *  @param item Where the item you are removing will be returned to.
         *  @param Timeout How long to wait to remove an item if the queue
         *         is currently empty.
         *  @return true if an item was copied, false if no item was copied.
         */
        bool Peek(void *item, TickType_t Timeout = portMAX_DELAY);

        /**
         *  Add an item to the back of the queue in ISR context.
         *
         *  @param item The item you are adding.
         *  @param pxHigherPriorityTaskWoken Did this operation result in a
         *         rescheduling event.
         *  @return true if the item was added, false if it was not.
         */
        virtual bool EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken);

        /**
         *  Remove an item from the front of the queue in ISR context.
         *
         *  @param item Where the item you are removing will be returned to.
         *  @param pxHigherPriorityTaskWoken Did this operation result in a
         *         rescheduling event.
         *  @return true if an item was removed, false if no item was removed.
         */
        bool DequeueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken);

        /**
         *  Make a copy of an item from the front of the queue. This will
         *  not remove it from the head of the queue.
         *
         *  @param item Where the item you are removing will be returned to.
         *  @return true if an item was copied, false if no item was copied.
         */
        bool PeekFromISR(void *item);

        /**
         *  Is the queue empty?
         *  @return true if the queue was empty when this was called, false if
         *  the queue was not empty.
         */
        bool IsEmpty();

        /**
         *  Is the queue full?
         *  @return true if the queue was full when this was called, false if
         *  the queue was not full.
         */
        bool IsFull();

        /**
         *  Remove all objects from the queue.
         */
        void Flush();

        /**
         *  How many items are currently in the queue.
         *  @return the number of items in the queue.
         */
        UBaseType_t NumItems();

        /**
         *  How many empty spaves are currently left in the queue.
         *  @return the number of remaining spaces.
         */
        UBaseType_t NumSpacesLeft();

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Not intended for use by application code.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        /**
         *  FreeRTOS queue handle.
         */
        QueueHandle_t handle;
};


/**
 *  Enhanced queue class that implements a double ended queue (a "deque"),
 *  almost. Unlike the traditional CommSci version, there is no way to
 *  dequeue from the back. Practically, this most likely isn't a big deal.
 *
 *  @note It is expected that an application will instantiate this class or
 *        one of the derived classes and use that. It is not expected that
 *        a user or application will derive from these classes.
 */
class Deque : public Queue {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Our constructor.
         *
         *  @throws QueueCreateException
         *  @param maxItems Maximum number of items thsi queue can hold.
         *  @param itemSize Size of an item in a queue.
         *  @note FreeRTOS queues use a memcpy / fixed size scheme for queues.
         */
        Deque(UBaseType_t maxItems, UBaseType_t itemSize);

        /**
         *  Add an item to the front of the queue. This will result in
         *  the item being removed first, ahead of all of the items
         *  added by the base calss Dequeue() function.
         *
         *  @param item The item you are adding.
         *  @param Timeout How long to wait to add the item to the queue if
         *         the queue is currently full.
         *  @return true if the item was added, false if it was not.
         */
        bool EnqueueToFront(void *item, TickType_t Timeout = portMAX_DELAY);

        /**
         *  Add an item to the front of the queue. This will result in
         *  the item being removed first, ahead of all of the items
         *  added by the base calss Dequeue() function.
         *
         *  @param item The item you are adding.
         *  @param pxHigherPriorityTaskWoken Did this operation result in a
         *         rescheduling event.
         *  @return true if the item was added, false if it was not.
         */
        bool EnqueueToFrontFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken);
};


/**
 *  Binary queue with overwrite. This queue can only hold one item.
 *  If sucessive Enqueue operations are called, that item is overwritten
 *  with whatever the last item was.
 *
 *  @note It is expected that an application will instantiate this class or
 *        one of the derived classes and use that. It is not expected that
 *        a user or application will derive from these classes.
 */
class BinaryQueue : public Queue {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Our constructor.
         *
         *  @throws QueueCreateException
         *  @param itemSize Size of an item in a queue.
         *  @note FreeRTOS queues use a memcpy / fixed size scheme for queues.
         */
        BinaryQueue(UBaseType_t itemSize);

         /**
          *  Add an item to the queue.
          *
          *  @param item The item you are adding.
          *  @return true always, because of overwrite.
          */
        virtual bool Enqueue(void *item);

         /**
          *  Add an item to the queue in ISR context.
          *
          *  @param item The item you are adding.
          *  @param pxHigherPriorityTaskWoken Did this operation result in a
          *         rescheduling event.
          *  @return true always, because of overwrite.
          */
        virtual bool EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken);
};


}
#endif
