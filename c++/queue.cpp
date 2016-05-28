#include "queue.hpp"


CQueue::CQueue(UBaseType_t maxItems, UBaseType_t itemSize)
{
    handle = xQueueCreate(maxItems, itemSize);

    if (handle == NULL) {
        throw CQueueCreationException();
    }
}


CQueue::~CQueue()
{
    vQueueDelete(handle);
}


bool CQueue::Enqueue(void *item)
{
    BaseType_t success;

    success = xQueueSendToBack(handle, item, portMAX_DELAY);

    return success == pdTRUE ? true : false;
}


bool CQueue::Enqueue(void *item, TickType_t xBlockTime)
{
    BaseType_t success;

    success = xQueueSendToBack(handle, item, xBlockTime);

    return success == pdTRUE ? true : false;
}


bool CQueue::Dequeue(void *item, TickType_t xBlockTime)
{
    BaseType_t success;

    success = xQueueReceive(handle, item, xBlockTime);

    return success == pdTRUE ? true : false;
}


bool CQueue::Peek(void *item, TickType_t xBlockTime)
{
    BaseType_t success;

    success = xQueuePeek(handle, item, xBlockTime);

    return success == pdTRUE ? true : false;
}


bool CQueue::EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xQueueSendToBackFromISR(handle, item, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


bool CQueue::DequeueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xQueueReceiveFromISR(handle, item, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


bool CQueue::PeekFromISR(void *item)
{
    BaseType_t success;

    success = xQueuePeekFromISR(handle, item);

    return success == pdTRUE ? true : false;
}


bool CQueue::IsEmpty()
{
    UBaseType_t cnt = uxQueueMessagesWaiting(handle);

    return cnt == 0 ? true : false;
}


bool CQueue::IsFull()
{
    UBaseType_t cnt = uxQueueSpacesAvailable(handle);

    return cnt == 0 ? true : false;
}


void CQueue::Flush()
{
    xQueueReset(handle);
}


UBaseType_t CQueue::NumItems()
{
    return uxQueueMessagesWaiting(handle);
}


UBaseType_t CQueue::NumSpacesLeft()
{
    return uxQueueSpacesAvailable(handle);
}


CDeque::CDeque(UBaseType_t maxItems, UBaseType_t itemSize)
    : CQueue(maxItems, itemSize)
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
    : CQueue(1, itemSize)
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



