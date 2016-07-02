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
#ifndef TICK_HOOK_HPP_
#define TICK_HOOK_HPP_

#if ( configUSE_TICK_HOOK == 1 )

#include "FreeRTOS.h"
#include "task.h"
#include <list>

extern "C" void vApplicationTickHook();


namespace cpp_freertos {


/**
 *  Wrapper class for Tick hooks, functions you want to run within 
 *  the tick ISR. 
 *
 *  This is an abstract base class.
 *  To use this, you need to subclass it. All of your tick functions 
 *  should be derived from this class. Then implement the virtual Run
 *  function. 
 */    
class TickHook {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //  Available from anywhere. 
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor.
         */
        TickHook();

        /**
         *  Destructor
         */
        virtual ~TickHook();

        /**
         *  After this is called your Run routine will execute in the 
         *  Tick ISR. This registration cannot be done in the constructor.
         */
        void Register();
        
        /**
         *  Disable the tick hook from running, without removing it 
         *  from the tick hook list.
         */
        void Disable();

        /**
         *  Enable this Idle Hook to run. This call is not necessary
         *  if you haven't called Disable.
         */
        void Enable();
        

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Available from inside your Idle task implementation.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        /**
         *  Implementation of your actual Tick Hook code.
         *  You must override this function.
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
         *  List of Tick Hook callbacks that are executed in the 
         *  Tick ISR.
         */
        static std::list<TickHook *>Callbacks;

        /**
         *  Should the tick hook run?
         */
        bool Enabled;

    /**
     *  Allow the global vApplicationTickHook() function access
     *  to the internals of this class. This simplifies the overall
     *  design.
     */
    friend void ::vApplicationTickHook();
};



}
#endif
#endif


