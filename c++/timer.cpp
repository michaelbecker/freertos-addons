#include "timer.hpp"


using namespace rtos_cpp;


Timer::Timer(   const char * const TimerName,
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


Timer::Timer(   TickType_t PeriodInTicks,
                bool Periodic
                )
{
    handle = xTimerCreate(  "Default",
                            PeriodInTicks,
                            Periodic ? pdTRUE : pdFALSE,
                            this,
                            TimerCallbackFunctionAdapter);

    if (handle == NULL) {
        throw TimerCreateException();
    }
}


Timer::~Timer()
{
    xTimerDelete(handle, portMAX_DELAY);
}


bool Timer::IsActive()
{
    return xTimerIsTimerActive(handle) == pdFALSE ? false : true;
}


bool Timer::Start(TickType_t Timeout)
{
    return xTimerStart(handle, Timeout) == pdFALSE ? false : true;
}


bool Stop(TickType_t Timeout)
{
    return xTimerStop(handle, Timeout) == pdFALSE ? false : true;
}


bool Timer::Reset(TickType_t Timeout)
{
    return xTimerReset(handle, Timeout) == pdFALSE ? false : true;
}


bool Timer::SetPeriod(  TickType_t NewPeriod,
                        TickType_t Timeout)
{
    return xTimerChangePeriod(handle, NewPeriod, Timeout) == pdFALSE
            ? false : true;
}



#if (INCLUDE_xTimerGetTimerDaemonTaskHandle == 1)

TaskHandle_t Timer::GetTimerDaemonHandle()
{
    return xTimerGetTimerDaemonTaskHandle();
}

#endif


void Timer::TimerCallbackFunctionAdapter(TimerHandle_t xTimer)
{
    Timer *timer = static_cast<Timer *>pvTimerGetTimerID(xTimer);
    timer->Run();
}
