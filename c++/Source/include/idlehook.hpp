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
#ifndef IDLEHOOK_HPP_
#define IDLEHOOK_HPP_

#if ( configUSE_IDLE_HOOK == 1 )

#include "FreeRTOS.h"
#include "task.h"
#include <list>


/**
 *  FreeRTOS expects this function to exist and requires it to be 
 *  named as such with the following signature.
 */
extern "C" void vApplicationIdleHook();


namespace cpp_freertos {


class IdleHook {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //  Available from anywhere. 
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor, after this is called your Run routine 
         *  will execute in the idle task.
         */
        IdleHook();

        /**
         *  You cannot directly call the destructor given the 
         *  design of the idle task. Therefore, to remove an 
         *  idle hook, we mark it for removal, and let the idle
         *  task delete it when it gets to it. After you call 
         *  this routine, you cannot touch the object anymore,
         *  it will be asynchronously deleted.
         */
        MarkForDelete();

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Available from inside your Idle task implementation.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        /**
         *  Implementation of your actual Idle code.
         *  You must override this function.
		 *  NOTE: YOUR RUN METHOD MUST NOT, UNDER ANY CIRCUMSTANCES,
		 *  CALL A FUNCTION THAT MIGHT BLOCK.
         */
        virtual void Run() = 0;

    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this wrapper class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:
        /**
         *  List of Idle Hook callbacks that are executed in the 
         *  idle task.
         */
        static std::list<IdleHook *>Callbacks;

        /**
         *  Request by a user to delete?
         */
        bool Cleanup;

        /**
         *  Destructor is private.
         */
        virtual ~IdleHook();

    /**
     *  Allow the global vApplicationIdleHook() function access
     *  to the internals of this class. This simplifies the overall
     *  design.
     */
    friend void ::vApplicationIdleHook();
};


}

#endif
#endif


