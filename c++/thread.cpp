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
#include "thread.hpp"


using namespace rtos_cpp;


Thread::Thread( const char * const pcName,
                uint16_t usStackDepth,
                UBaseType_t uxPriority)
{
#if (INCLUDE_vTaskDelayUntil == 1)
    delayUntilInitialized = false;
#endif

    if (pcName == NULL)
        pcName = "Default";

    BaseType_t rc = xTaskCreate(TaskFunctionAdapter,
                                pcName,
                                usStackDepth,
                                this,
                                uxPriority,
                                &handle);
    if (rc != pdPASS) {
        throw ThreadCreateException(rc);
    }
}


Thread::Thread( uint16_t usStackDepth,
                UBaseType_t uxPriority)
{
#if (INCLUDE_vTaskDelayUntil == 1)
    delayUntilInitialized = false;
#endif

    BaseType_t rc = xTaskCreate(TaskFunctionAdapter,
                                "Default",
                                usStackDepth,
                                this,
                                uxPriority,
                                &handle);
    if (rc != pdPASS) {
        throw ThreadCreateException(rc);
    }
}


#if (INCLUDE_vTaskDelete == 1)

Thread::~Thread()
{
    vTaskDelete(handle);
    handle = -1;
}

#endif


void Thread::TaskFunctionAdapter(void *pvParameters)
{
    Thread *thread = static_cast<Thread *>(pvParameters);

    thread->run();

#if (INCLUDE_vTaskDelete == 1)

    vTaskDelete(thread->handle);

#else
    configASSERT( ! "Cannot return from a thread.run function "
                    "if INCLUDE_vTaskDelete is not defined.");
#endif
}


#if (INCLUDE_vTaskDelayUntil == 1)

void Thread::DelayUntil(const TickType_t Period)
{
    if (!delayUntilInitialized) {
        delayUntilInitialized = true;
        delayUntilPreviousWakeTime = xTaskGetTickCount();
    }

    vTaskDelayUntil(&delayUntilPreviousWakeTime, Period);
}


void Thread::ResetDelayUntil()
{
    delayUntilInitialized = false;
}

#endif
