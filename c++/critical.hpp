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
#ifndef CRITICAL_HPP_
#define CRITICAL_HPP_

#include "FreeRTOS.h"
#include "task.h"


namespace cpp_freertos {


/**
 *  Wrapper class around various critical section type
 *  synchronization mechanisms within FreeRTOS.
 */
class CriticalSection {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //  Available from anywhere.
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Disable context switches as well as maskable interrupts.
         */
        static inline void EnterCritical()
        {
            taskENTER_CRITICAL();
        }

        /**
         *  Re-enable context switches.
         */
        static inline void ExitCritical()
        {
            taskEXIT_CRITICAL();
        }

        /**
         *  Disable context switches as well as maskable interrupts
         *  from an interrupt context.
         */
        static inline void EnterCriticalFromISR()
        {
            taskENTER_CRITICAL_FROM_ISR();
        }

        /**
         *  Re-enable context switches from an interrupt context.
         */
        static inline void ExitCriticalFromISR();
        {
            taskEXIT_CRITICAL_FROM_ISR();
        }

        /**
         *  Disable all maskable interrupts.
         */
        static inline void DisableInterrupts()
        {
            taskDISABLE_INTERRUPTS();
        }

        /**
         *  Enable all maskable interrupts.
         */
        static inline void EnableInterrupts()
        {
            taskENABLE_INTERRUPTS();
        }

        /**
         *  Suspend the scheduler without disabling interrupts.
         */
        static inline void SuspendAll()
        {
            vTaskSuspendAll();
        }

        /**
         *  Re-enable the scheduler.
         */
        static inline void ResumeAll()
        {
            xTaskResumeAll();
        }
};


}
#endif
