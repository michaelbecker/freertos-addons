/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
 *
 *  The FreeRTOS C++ Wrappers project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2
 *  of the License, or (at your option) any later version.
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
#include "timer.hpp"


using namespace cpp_freertos;


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


bool Timer::Stop(TickType_t CmdTimeout)
{
    return xTimerStop(handle, CmdTimeout) == pdFALSE ? false : true;
}


bool Timer::StopFromISR(BaseType_t *pxHigherPriorityTaskWoken)
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
    Timer *timer = static_cast<Timer *>(pvTimerGetTimerID(xTimer));
    timer->Run();
}
