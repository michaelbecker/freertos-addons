/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
 *
 *  The FreeRTOS C++ Wrappers project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 3 of the License.
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
#include "idlehook.hpp"

#if ( configUSE_IDLE_HOOK == 1 )


using namespace std;
using namespace cpp_freertos;


list<IdleHook *> IdleHook::Callbacks;


IdleHook::IdleHook()
    : Cleanup(false), Enabled(true)
{
}


void IdleHook::Register()
{
    vTaskSuspendAll();
    Callbacks.push_front(this);
    xTaskResumeAll();
}


void IdleHook::MarkForDelete()
{
    vTaskSuspendAll();
    Cleanup = true;
    xTaskResumeAll();
}


IdleHook::~IdleHook()
{
    vTaskSuspendAll();
    Callbacks.remove(this);
    TaskResumeAll();
}


void IdleHook::Disable()
{
    vTaskSuspendAll();
    Enabled = false;
    xTaskResumeAll();
}


void IdleHook::Enable()
{
    vTaskSuspendAll();
    Enabled = true;
    xTaskResumeAll();
}


/**
 *  We are a friend of the IdleHook class, which makes this much simplier.
 */
void vApplicationIdleHook(void)
{
    //
    //  Making a copy of the list protects the list when 
    //  hooks are added to it. Using the Cleanup flag 
    //  protects when items are removed from it.
    //
    vTaskSuspendAll();
    list<IdleHook *>local_callbacks = IdleHook::Callbacks;
    xTaskResumeAll();
    
    for (list<IdleHook *>::iterator it = local_callbacks.begin();
         it != local_callbacks.end();
         ++it) {

        IdleHook *idleHookObject = *it;

        //
        //  If a user asked for this to be deleted, free it
        //  and remove it from the list.
        //
        if (idleHookObject->Cleanup) {
            delete idleHookObject;
        }
        else if (idleHookObject->Enabled){
            idleHookObject->Run();
        }
    }
}

#endif


