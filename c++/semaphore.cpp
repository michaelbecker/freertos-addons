/****************************************************************************
 *
 ***************************************************************************/
#include "semaphore.hpp"


using namespace rtos_cpp;


bool Semaphore::Take(TickType_t xBlockTime)
{
    BaseType_t success;

    success = xSemaphoreTake(handle, xBlockTime);

    return success == pdTRUE ? true : false;
}


bool Semaphore::TakeFromISR(BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xSemaphoreTakeFromISR(handle, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


bool Semaphore::Give()
{
    BaseType_t success;

    success = xSemaphoreGive(handle);

    return success == pdTRUE ? true : false;
}


bool Semaphore::GiveFromISR(BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t success;

    success = xSemaphoreGiveFromISR(handle, pxHigherPriorityTaskWoken);

    return success == pdTRUE ? true : false;
}


Semaphore::~Semaphore()
{
    vSemaphoreDelete(handle);
}


BinarySemaphore::BinarySemaphore(bool set)
{
    handle = xSemaphoreCreateBinary();

    if (handle == NULL) {
        throw SemaphoreCreateException();
    }

    if (set) {
        xSemaphoreGive(handle);
    }
}


CountingSemaphore::CountingSemaphore(UBaseType_t maxCount, UBaseType_t initialCount)
{
    if (maxCount == 0) {
        throw SemaphoreCreateException("bad maxCount");
    }

    if (initialCount > maxCount) {
        throw SemaphoreCreateException("bad initialCount");
    }

    handle = xSemaphoreCreateCounting(maxCount, initialCount);

    if (handle == NULL) {
        throw SemaphoreCreateException();
    }
}
