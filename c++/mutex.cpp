#include "mutex.hpp"


using namespace rtos_cpp;


Mutex::~Mutex()
{
    vSemaphoreDelete(handle);
}


MutexStandard::MutexStandard()
{
    handle = xSemaphoreCreateMutex();

    if (handle == NULL) {
        throw MutexCreateException();
    }
}


bool MutexStandard::Lock(TickType_t Timeout)
{
    BaseType_t success = xSemaphoreTake(handle, Timeout);
    return success == pdTRUE ? true : false;
}


bool MutexStandard::Unlock()
{
    BaseType_t success = xSemaphoreGive(handle);
    return success == pdTRUE ? true : false;
}


#if (configUSE_RECURSIVE_MUTEXES == 1)

MutexRecursive::MutexRecursive()
{
    handle = xSemaphoreCreateRecursiveMutex();

    if (handle == NULL) {
        throw MutexCreateException();
    }
}


bool MutexRecursive::Lock(TickType_t Timeout)
{
    BaseType_t success = xSemaphoreTakeRecursive(handle, Timeout);
    return success == pdTRUE ? true : false;
}


bool MutexRecursive::Unlock()
{
    BaseType_t success = xSemaphoreGiveRecursive(handle);
    return success == pdTRUE ? true : false;
}

#endif


LockGuard::LockGuard(Mutex& m)
    : mutex(m)
{
    mutex.Lock();
}


LockGuard::~LockGuard()
{
    mutex.Unlock();
}
