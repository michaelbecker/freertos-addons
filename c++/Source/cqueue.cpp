/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
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
 ***************************************************************************/
#include "queue.hpp"


using namespace cpp_freertos;


Queue::Queue(UBaseType_t maxItems, UBaseType_t itemSize)
{
    handle = xQueueCreate(maxItems, itemSize);

    if (handle == NULL) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw QueueCreateException();
#else
        configASSERT(!"Queue Constructor Failed");
#endif
    }
}


Queue::~Queue()
{
    vQueueDelete(handle);
}


bool Queue::Enqueue(void *item)
{
    BaseType_t success;

    success = xQueueSendToBack(handle, item, portMAX_DELAY);

    return success == pdTRUE ? true : false;
}


bool Queue::Enqueue(void *item, TickType_t Timeout)
{
    BaseType_t success;

    success = xQueueSendToBack(handle, item, Timeout);

    return success == pdTRUE ? true : false;
}


bool Queue::Dequeue(void *item, TickType_t Timeout)
{
    BaseType_t success;

    success = xQueueReceive(handle, item, Timeout);

    return success == pdTRUE ? true : false;
}


bool Queue::Peek(void *item, TickType_t Timeout)
{
    BaseType_t success;

    success = xQueuePeek(handle, item, Timeout);

    return success == pdTRUE ? true : false;
}


bool Queue::EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xQueueSendToBackFromISR(handle, item, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


bool Queue::DequeueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xQueueReceiveFromISR(handle, item, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


bool Queue::PeekFromISR(void *item)
{
    BaseType_t success;

    success = xQueuePeekFromISR(handle, item);

    return success == pdTRUE ? true : false;
}


bool Queue::IsEmpty()
{
    UBaseType_t cnt = uxQueueMessagesWaiting(handle);

    return cnt == 0 ? true : false;
}


bool Queue::IsFull()
{
    UBaseType_t cnt = uxQueueSpacesAvailable(handle);

    return cnt == 0 ? true : false;
}


void Queue::Flush()
{
    xQueueReset(handle);
}


UBaseType_t Queue::NumItems()
{
    return uxQueueMessagesWaiting(handle);
}


UBaseType_t Queue::NumSpacesLeft()
{
    return uxQueueSpacesAvailable(handle);
}


Deque::Deque(UBaseType_t maxItems, UBaseType_t itemSize)
    : Queue(maxItems, itemSize)
{
}


bool Deque::EnqueueToFront(void *item, TickType_t Timeout)
{
    BaseType_t success;

    success = xQueueSendToFront(handle, item, Timeout);

    return success == pdTRUE ? true : false;
}


bool Deque::EnqueueToFrontFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xQueueSendToFrontFromISR(handle, item, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


BinaryQueue::BinaryQueue(UBaseType_t itemSize)
    : Queue(1, itemSize)
{
}


bool BinaryQueue::Enqueue(void *item)
{
    (void)xQueueOverwrite(handle, item);
    return true;
}


bool BinaryQueue::EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken)
{
    (void)xQueueOverwriteFromISR(handle, item, pxHigherPriorityTaskWoken);
    return true;
}
