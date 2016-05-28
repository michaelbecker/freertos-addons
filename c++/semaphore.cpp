/****************************************************************************
 *
 ***************************************************************************/
#include "semaphore.hpp"


bool CSemaphore::Take(TickType_t xBlockTime)
{
    BaseType_t success;

    success = xSemaphoreTake(handle, xBlockTime);

    return success == pdTRUE ? true : false;
}


bool CSemaphore::TakeFromISR(BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xSemaphoreTakeFromISR(handle, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


bool CSemaphore::Give()
{
    BaseType_t success;

    success = xSemaphoreGive(handle);

    return success == pdTRUE ? true : false;
}


bool CSemaphore::GiveFromISR(BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xSemaphoreGiveFromISR(handle, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


CSemaphore::~CSemaphore()
{
    vSemaphoreDelete(handle);
}


CBinarySemaphore::CBinarySemaphore(bool set)
{
    handle = xSemaphoreCreateBinary();

    if (handle == NULL) {
        throw CSemaphoreCreationException();
    }

    if (set) {
        xSemaphoreGive(handle);
    }
}


CCountingSemaphore::CCountingSemaphore(UBaseType_t maxCount, UBaseType_t initialCount)
{
    if (maxCount == 0) {
        throw CSemaphoreCreationException("bad maxCount");
    }

    if (initialCount > maxCount) {
        throw CSemaphoreCreationException("bad initialCount");
    }

    handle = xSemaphoreCreateCounting(maxCount, initialCount);

    if (handle == NULL) {
        throw CSemaphoreCreationException();
    }
}


