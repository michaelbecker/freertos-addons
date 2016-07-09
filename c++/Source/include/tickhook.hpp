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

#include "FreeRTOS.h"
#include "task.h"
#include <list>

#if ( configUSE_TICK_HOOK == 1 )

/**
 *  FreeRTOS expects this function to exist and requires it to be 
 *  named as such with the following signature.
 */
extern "C" void vApplicationTickHook(void);

namespace cpp_freertos {

/**
 *  Wrapper class for Tick hooks, functions you want to run within 
 *  the tick ISR. 
 *
 *  This is an abstract base class.
 *  To use this, you need to subclass it. All of your tick functions 
 *  should be derived from this class. Then implement the virtual Run
 *  function. 
 *
 *  You can register multiple hooks with this class. The order of 
 *  execution should not be assumed. All tick hooks will execute 
 *  every tick.
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
         *  Tick ISR. This registration cannot be done in the base class 
         *  constructor. Once your object is fully constructed, you "may"
         *  call this in your derived class's constructor.
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


