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
#include "mutex.hpp"


using namespace cpp_freertos;


Mutex::Mutex()
{
}


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
