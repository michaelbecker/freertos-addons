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
#ifndef TASKLET_HPP_
#define TASKLET_HPP_


/**
 *  C++ exceptions are used by default when constructors fail.
 *  If you do not want this behavior, define the following in your makefile
 *  or project. Note that in most / all cases when a constructor fails,
 *  it's a fatal error. In the cases when you've defined this, the new 
 *  default behavior will be to issue a configASSERT() instead.
 */
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
#include <exception>
#include <string>
#include <cstdio>
#ifdef CPP_FREERTOS_NO_CPP_STRINGS
#error "FreeRTOS-Addons require C++ Strings if you are using exceptions"
#endif
#endif
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"


namespace cpp_freertos {


#ifndef CPP_FREERTOS_NO_EXCEPTIONS
/**
 *  This is the exception that is thrown if a Tasklet constructor fails.
 */
class TaskletCreateException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        TaskletCreateException()
        {
            sprintf(errorString, "Tasklet Constructor Failed");
        }

        /**
         *  Create the exception.
         */
        explicit TaskletCreateException(const char *info)
        {
            snprintf(errorString, sizeof(errorString),
                        "Tasklet Constructor Failed %s", info);
        }

        /**
         *  Get what happened as a string.
         *  We are overriding the base implementation here.
         */
        virtual const char *what() const throw()
        {
            return errorString;
        }

    private:
        /**
         *  A text string representing what failed.
         */
        char errorString[80];
};
#endif


/**
 *  A FreeRTOS wrapper for its concept of a Pended Function.
 *  In Linux, one permutation of this would be a Tasklet, or
 *  bottom half processing from an ISR.
 *
 *  This is an abstract base class.
 *  To use this, you need to subclass it. All of your Tasklets should
 *  be derived from the Tasklet class. Then implement the virtual Run
 *  function. This is a similar design to Java threading.
 */
class Tasklet {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor
         *  @note Do not construct inside an ISR! This includes creating 
         *  local instances of this object.
         */
        Tasklet();

        /**
         *  Destructor
         *  @note Do not delete inside an ISR! This includes the automatic 
         *  deletion of local instances of this object when leaving scope.
         */
        virtual ~Tasklet();

        /**
         *  Schedule this Tasklet to run.
         *
         *  @param parameter Value passed to your Run method.
         *  @param CmdTimeout How long to wait to send this command to the
         *         timer daemon.
         *  @returns true if this command will be sent to the timer daemon,
         *           false if it will not (i.e. timeout).
         */
        bool Schedule(  uint32_t parameter,
                        TickType_t CmdTimeout = portMAX_DELAY);

        /**
         *  Schedule this Tasklet to run from ISR context.
         *  This allows FreeRTOS ISRs to defer processing from the ISR
         *  into a task context.
         *
         *  @param parameter Value passed to your Run method.
         *  @param pxHigherPriorityTaskWoken Did this operation result in a
         *         rescheduling event.
         *  @returns true if this command will be sent to the timer daemon,
         *           false if it will not (i.e. timeout).
         */
        bool ScheduleFromISR(   uint32_t parameter,
                                BaseType_t *pxHigherPriorityTaskWoken);

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Available from inside your Tasklet implementation.
    //  You should make sure that you are only calling these methods
    //  from within your Run() method, or that your Run() method is on the
    //  callstack.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        /**
         *  Implementation of your actual tasklet code.
         *  You must override this function.
         *
         *  @param parameter Value passed to you from the Schedule() methods.
         */
        virtual void Run(uint32_t parameter) = 0;

        /**
         *  You must call this in your dtor, to synchronize between 
         *  being called and being deleted.
         */
        void CheckForSafeDelete();

    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this wrapper class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:
        /**
         *  Adapter function that allows you to write a class
         *  specific Run() function that interfaces with FreeRTOS.
         *  Look at the implementation of the constructors and this
         *  code to see how the interface between C and C++ is performed.
         */
        static void TaskletAdapterFunction(void *ref, uint32_t parameter);

        /**
         *  Protect against accidental deletion before we were executed.
         */
        SemaphoreHandle_t DtorLock;
};

}
#endif

