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
#ifndef TASKLET_HPP_
#define TASKLET_HPP_


#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"
// TODO - explore replacing sprintf with stringstream
#include <cstdio>


namespace cpp_freertos {


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
