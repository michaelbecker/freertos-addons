#ifndef TIMER_HPP_
#define TIMER_HPP_


#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "timers.h"


namespace cpp_freertos {


/**
 *  This is the exception that is thrown if a Thread constructor fails.
 */
class TimerCreateException  : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        TimerCreateException(BaseType_t error)
        {
            errorCode = error;
            sprintf(errorString, "Timer Constructor Failed %d", errorCode);
        }

        /**
         *  Get what happened as a string.
         *  We are overriding the base implementation here.
         */
        virtual const char *what() const throw()
        {
            return errorString;
        }

        /**
         *  Get the FreeRTOS error code.
         */
        BaseType_t getErrorCode()
        {
            return errorCode;
        }

    private:
        /**
         *  Save the FreeRTOS return code code from xTaskCreate().
         */
        BaseType_t errorCode;

        /**
         *  A text string representing what failed.
         */
        char errorString[40];
};


class Timer {

    public:
        Timer(  const char * const TimerName,
                TickType_t PeriodInTicks,
                bool Periodic = true
                );

        bool IsActive();

        bool Start();

#if (INCLUDE_xTimerGetTimerDaemonTaskHandle == 1)
        static TaskHandle_t GetTimerDaemonHandle();
#endif

    protected:
        virtual void TimerCallback() = 0;

    private:
        TimerHandle_t handle;
        static void TimerCallbackFunctionAdapter(TimerHandle_t xTimer);

};


}
#endif
