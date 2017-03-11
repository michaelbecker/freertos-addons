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
         *  @note Immedately after you call this function, your TickHook
         *  Run() method will run, perhaps before you even return from this 
         *  call. You "must" be ready to run before you call Register().
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


