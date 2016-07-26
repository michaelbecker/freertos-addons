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
#include "tickhook.hpp"

#if ( configUSE_TICK_HOOK == 1 )

using namespace std;
using namespace cpp_freertos;


list<TickHook *> TickHook::Callbacks;


TickHook::TickHook()
    : Enabled(true)
{
}


TickHook::~TickHook()
{
    taskENTER_CRITICAL();
    Callbacks.remove(this);
    taskEXIT_CRITICAL();
}


void TickHook::Register()
{
    taskENTER_CRITICAL();
    Callbacks.push_front(this);
    taskEXIT_CRITICAL();
}


void TickHook::Disable()
{
    taskENTER_CRITICAL();
    Enabled = false;
    taskEXIT_CRITICAL();
}


void TickHook::Enable()
{
    taskENTER_CRITICAL();
    Enabled = true;
    taskEXIT_CRITICAL();
}


/**
 *  We are a friend of the Tick class, which makes this much simplier.
 */
void vApplicationTickHook(void)
{
    for (list<TickHook *>::iterator it = TickHook::Callbacks.begin();
         it != TickHook::Callbacks.end();
         ++it) {

        TickHook *tickHookObject = *it;

        if (tickHookObject->Enabled){
            tickHookObject->Run();
        }
    }
}

#endif


