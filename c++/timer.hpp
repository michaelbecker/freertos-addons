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

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        Timer(  const char * const TimerName,
                TickType_t PeriodInTicks,
                bool Periodic = true
                );

        Timer(  TickType_t PeriodInTicks,
                bool Periodic = true
                );

        ~Timer();

        bool IsActive();

        bool Start(TickType_t Timeout = portMAX_DELAY);

        bool Stop(TickType_t Timeout = portMAX_DELAY);

        bool Reset(TickType_t Timeout = portMAX_DELAY);

        bool SetPeriod( TickType_t NewPeriod,
                        TickType_t Timeout = portMAX_DELAY);

///     TODO - Get ISR versions here

#if (INCLUDE_xTimerGetTimerDaemonTaskHandle == 1)
        static TaskHandle_t GetTimerDaemonHandle();
#endif

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Available from inside your Thread implementation.
    //  You should make sure that you are only calling these methods
    //  from within your Run() method, or that your Run() method is on the
    //  callstack.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        virtual Run() = 0;

    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this wrapper class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:
        TimerHandle_t handle;
        static void TimerCallbackFunctionAdapter(TimerHandle_t xTimer);

};


}
#endif
