/****************************************************************************
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
