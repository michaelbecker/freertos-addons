#include "queue.hpp"


using namespace rtos_cpp;


Queue::Queue(UBaseType_t maxItems, UBaseType_t itemSize)
{
    handle = xQueueCreate(maxItems, itemSize);

    if (handle == NULL) {
        throw QueueCreateException();
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


bool Queue::Enqueue(void *item, TickType_t xBlockTime)
{
    BaseType_t success;

    success = xQueueSendToBack(handle, item, xBlockTime);

    return success == pdTRUE ? true : false;
}


bool Queue::Dequeue(void *item, TickType_t xBlockTime)
{
    BaseType_t success;

    success = xQueueReceive(handle, item, xBlockTime);

    return success == pdTRUE ? true : false;
}


bool Queue::Peek(void *item, TickType_t xBlockTime)
{
    BaseType_t success;

    success = xQueuePeek(handle, item, xBlockTime);

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


CDeque::CDeque(UBaseType_t maxItems, UBaseType_t itemSize)
    : Queue(maxItems, itemSize)
{
}


bool CDeque::EnqueueToFront(void *item, TickType_t xBlockTime)
{
    BaseType_t success;

    success = xQueueSendToFront(handle, item, xBlockTime);

    return success == pdTRUE ? true : false;
}


bool CDeque::EnqueueToFrontFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xQueueSendToFrontFromISR(handle, item, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


CBinaryQueue::CBinaryQueue(UBaseType_t itemSize)
    : Queue(1, itemSize)
{
}


bool CBinaryQueue::Enqueue(void *item)
{
    (void)xQueueOverwrite(handle, item);
    return true;
}


bool CBinaryQueue::EnqueueFromISR(const void *item, BaseType_t *pxHigherPriorityTaskWoken)
{
    (void)xQueueOverwriteFromISR(handle, item, pxHigherPriorityTaskWoken);
    return true;
}
