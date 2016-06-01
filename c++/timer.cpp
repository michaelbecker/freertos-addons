#include "timer.hpp"


using namespace rtos_cpp;


Timer::Timer(  const char * const TimerName,
                TickType_t PeriodInTicks,
                bool Periodic
                )
{
    handle = xTimerCreate(  TimerName,
                            PeriodInTicks,
                            Periodic ? pdTRUE : pdFALSE,
                            this,
                            TimerCallbackFunctionAdapter);

    if (handle == NULL) {
        throw TimerCreateException();
    }
}


bool Timer::IsActive()
{
    return xTimerIsTimerActive(handle) == pdFALSE ? false : true;
}


#if (INCLUDE_xTimerGetTimerDaemonTaskHandle == 1)

TaskHandle_t Timer::GetTimerDaemonHandle()
{
    return xTimerGetTimerDaemonTaskHandle();
}

#endif
