/****************************************************************************
 *
 *  Copyright (c) 2023, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS Add-ons project.
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
 *  MIT License
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so,subject to the
 *  following conditions:
 *
 *  + The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *  + Credit is appreciated, but not required, if you find this project
 *    useful enough to include in your application, product, device, etc.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
    #ifdef ESP_PLATFORM
        portMUX_TYPE mux;
        taskENTER_CRITICAL(&mux);
        Callbacks.remove(this);
        taskEXIT_CRITICAL(&mux);
    #else
        taskENTER_CRITICAL();
        Callbacks.remove(this);
        taskEXIT_CRITICAL();
    #endif
}


void TickHook::Register()
{
    #ifdef ESP_PLATFORM
        portMUX_TYPE mux;
        taskENTER_CRITICAL(&mux);
        Callbacks.push_front(this);
        taskEXIT_CRITICAL(&mux);
    #else
        taskENTER_CRITICAL();
        Callbacks.push_front(this);
        taskEXIT_CRITICAL();
    #endif
}


void TickHook::Disable()
{
    #ifdef ESP_PLATFORM
        portMUX_TYPE mux;
        taskENTER_CRITICAL(&mux);
        Enabled = false;
        taskEXIT_CRITICAL(&mux);
    #else
        taskENTER_CRITICAL();
        Enabled = false;
        taskEXIT_CRITICAL();
    #endif

}


void TickHook::Enable()
{
    #ifdef ESP_PLATFORM
        portMUX_TYPE mux;
        taskENTER_CRITICAL(&mux);
        Enabled = true;
        taskEXIT_CRITICAL(&mux);
    #else
        taskENTER_CRITICAL();
        Enabled = true;
        taskEXIT_CRITICAL();
    #endif

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


