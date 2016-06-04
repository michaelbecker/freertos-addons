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


bool Timer::Start(TickType_t CmdTimeout)
{
    return xTimerStart(handle, CmdTimeout) == pdFALSE ? false : true;
}


bool Timer::StartFromISR(BaseType_t *pxHigherPriorityTaskWoken)
{
    return xTimerStartFromISR(handle, pxHigherPriorityTaskWoken) == pdFALSE
            ? false : true;
}


bool Stop(TickType_t CmdTimeout)
{
    return xTimerStop(handle, CmdTimeout) == pdFALSE ? false : true;
}


bool StopFromISR(BaseType_t *pxHigherPriorityTaskWoken)
{
    return xTimerStopFromISR(handle, pxHigherPriorityTaskWoken) == pdFALSE
            ? false : true;
}


bool Timer::Reset(TickType_t CmdTimeout)
{
    return xTimerReset(handle, CmdTimeout) == pdFALSE ? false : true;
}


bool Timer::ResetFromISR(BaseType_t *pxHigherPriorityTaskWoken)
{
    return xTimerResetFromISR(handle, pxHigherPriorityTaskWoken) == pdFALSE
            ? false : true;
}


bool Timer::SetPeriod(  TickType_t NewPeriod,
                        TickType_t CmdTimeout)
{
    return xTimerChangePeriod(handle, NewPeriod, CmdTimeout) == pdFALSE
            ? false : true;
}


bool Timer::SetPeriodFromISR(   TickType_t NewPeriod,
                                BaseType_t *pxHigherPriorityTaskWoken)
{
    return xTimerChangePeriodFromISR(   handle, NewPeriod,
                                        pxHigherPriorityTaskWoken) == pdFALSE
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


