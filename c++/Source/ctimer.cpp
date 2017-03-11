/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
 *  
 *  Source Code:
 *  https://github.com/michaelbecker/freertos-addons
 *
 *  Project Page:
 *  http://michaelbecker.github.io/freertos-addons/
 *
 *  On-line Documentation:
 *  http://michaelbecker.github.io/freertos-addons/docs/html/index.html
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
 *  NOTE: A modification to the GPL is included to allow you to distribute a
 *  combined work that includes FreeRTOS C++ Wrappers project
 *  without being obliged to provide the source
 *  code for proprietary components.
 * 
 *  The FreeRTOS C++ Wrappers project GPL exception text follows:
 * 
 *  Any FreeRTOS C++ Wrapper *source code*, whether modified or in it's 
 *  original release form, or whether in whole or in part, can only be 
 *  distributed by you under the terms of the GNU General Public License plus
 *  this exception.  An independent module is a module which is not derived
 *  from or based on FreeRTOS C++ Wrappers project.
 * 
 *  Clause 1:
 * 
 *  Linking FreeRTOS C++ Wrappers project  with other modules is making a 
 *  combined work based on FreeRTOS C++ Wrappers project. Thus, the terms 
 *  and conditions of the GNU General Public License V2 cover the
 *  whole combination.
 * 
 *  As a special exception, the copyright holders of FreeRTOS C++ Wrappers 
 *  project give you permission to link FreeRTOS C++ Wrappers project with 
 *  independent modules to produce a statically linked executable, regardless
 *  of the license terms of these independent modules, and to copy and 
 *  distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the 
 *  terms and conditions of the license of that module.  An independent 
 *  module is a module which is not derived from or based on FreeRTOS C++ 
 *  Wrappers project.
 * 
 *  Clause 2:
 * 
 *  FreeRTOS C++ Wrappers project may not be used for any competitive or 
 *  comparative purpose, including the publication of any form of run time 
 *  or compile time metric, without the express permission of the copyright 
 *  holder(s) (this is the norm within the industry and is intended to ensure
 *  information accuracy).
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
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw TimerCreateException();
#else
        configASSERT(!"Timer Constructor Failed");
#endif
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
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw TimerCreateException();
#else
        configASSERT(!"Timer Constructor Failed");
#endif
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
