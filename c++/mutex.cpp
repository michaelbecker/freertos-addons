#include "mutex.hpp"


CMutex::~CMutex()
{
    vSemaphoreDelete(handle);
}


CMutexStandard::CMutexStandard()
{
    handle = xSemaphoreCreateMutex();

    if (handle == NULL) {
        throw CMutexCreationException();
    }
}


bool CMutexStandard::Lock(TickType_t xBlockTime)
{
    BaseType_t success = xSemaphoreTake(handle, xBlockTime);
    return success == pdTRUE ? true : false;
}


bool CMutexStandard::Unlock()
{
    BaseType_t success = xSemaphoreGive(handle);
    return success == pdTRUE ? true : false;
}


#if (configUSE_RECURSIVE_MUTEXES == 1)

CMutexRecursive::CMutexRecursive()
{
    handle = xSemaphoreCreateRecursiveMutex();

    if (handle == NULL) {
        throw CMutexCreationException();
    }
}


bool CMutexRecursive::Lock(TickType_t xBlockTime)
{
    BaseType_t success = xSemaphoreTakeRecursive(handle, xBlockTime);
    return success == pdTRUE ? true : false;
}


bool CMutexRecursive::Unlock()
{
    BaseType_t success = xSemaphoreGiveRecursive(handle);
    return success == pdTRUE ? true : false;
}

#endif


CLockGuard::CLockGuard(CMutex& m)
    : mutex(m)
{
    mutex.Lock();
}


CLockGuard::~CLockGuard()
{
    mutex.Unlock();
}


