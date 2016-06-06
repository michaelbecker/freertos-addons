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
