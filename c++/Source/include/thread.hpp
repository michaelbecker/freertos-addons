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
#ifndef THREAD_HPP_
#define THREAD_HPP_

/**
 *  The default in the C++ Wrapper classes is to use the C++ string class. 
 *  If you do not want this, define the following in your makefile or 
 *  project, and the Thread class will default to using character arrays
 *  instead of C++ strings.
 *
 *  @note If you define this, you also must define CPP_FREERTOS_NO_EXCEPTIONS.
 *  Some classes throw exceptions if they cannot be constructed, and the 
 *  exceptions they throw depend on C++ strings.
 */
#ifndef CPP_FREERTOS_NO_CPP_STRINGS
#include <string>
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "mutex.hpp"
#include "semaphore.hpp"
#include "condition_variable.hpp"

namespace cpp_freertos {


/**
 *  Wrapper class around FreeRTOS's implementation of a task.
 *
 *  This is an abstract base class.
 *  To use this, you need to subclass it. All of your threads should
 *  be derived from the Thread class. Then implement the virtual Run
 *  function. This is a similar design to Java threading.
 *
 *  By default, we leverage C++ strings for the Thread Name. If this
 *  is not desirable, define CPP_FREERTOS_NO_CPP_STRINGS and the class
 *  will fall back to C character arrays.
 */
class Thread {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //  Available from anywhere. Many of these require a Thread reference
    //  if they are operating on a thread.
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor to create a named thread.
         *
         *  @param Name Name of the thread. Only useful for debugging.
         *  @param StackDepth Number of "words" allocated for the Thread stack.
         *  @param Priority FreeRTOS priority of this Thread.
         */
#ifndef CPP_FREERTOS_NO_CPP_STRINGS
        Thread( const std::string Name,
                uint16_t StackDepth,
                UBaseType_t Priority);
#else
        Thread( const char *Name,
                uint16_t StackDepth,
                UBaseType_t Priority);
#endif

        /**
         *  Constructor to create an unnamed thread.
         *
         *  @param StackDepth Number of "words" allocated for the Thread stack.
         *  @param Priority FreeRTOS priority of this Thread.
         */
        Thread( uint16_t StackDepth,
                UBaseType_t Priority);

        /**
         *  Starts a thread.
         *
         *  This is the API call that actually starts the thread running. 
         *  It creates a backing FreeRTOS task. By separating object creation 
         *  from starting the Thread, it solves the pure virtual fuction call 
         *  failure case. If we attempt to automatically call xTaskCreate 
         *  from the base class constructor, in certain conditions the task 
         *  starts to run "before" the derived class is constructed! So we 
         *  don't do that anymore.
         *
         *  This may be called from your ctor once you have completed 
         *  your objects construction (so as the last step). 
         *
         *  This should only be called once ever! 
         */
        bool Start();

        /**
         *  Our destructor. This must exist even if FreeRTOS is
         *  configured to disallow task deletion.
         */
        virtual ~Thread();

        /**
         *  Accessor to get the thread's backing task handle.
         *  There is no setter, on purpose.
         *
         *  @return FreeRTOS task handle.
         */
        inline TaskHandle_t GetHandle()
        {
            return handle;
        }

        /**
         *  Yield the scheduler.
         */
        static inline void Yield()
        {
            taskYIELD();
        }

        /**
         *  Start the scheduler.
         *
         *  @note You need to use this call. Do NOT directly call 
         *  vTaskStartScheduler while using this library.
         */
        static inline void StartScheduler()
        {
            SchedulerActive = true;
            vTaskStartScheduler();
        }

        /**
         *  End the scheduler.
         *
         *  @note Please see the FreeRTOS documentation regarding constraints
         *  with the implementation of this.
         *
         *  @note You need to use this call. Do NOT directly call 
         *  vTaskEndScheduler while using this library.
         */
        static inline void EndScheduler()
        {
            vTaskEndScheduler();
            SchedulerActive = false;
        }

#if (INCLUDE_vTaskSuspend == 1)
        /**
         *  Suspend this thread.
         *
         *  @note While a Thread can Suspend() itself, it cannot Resume() 
         *  itself, becauseit's suspended.
         */
        inline void Suspend()
        {
            vTaskSuspend(GetHandle());
        }

        /**
         *  Resume a specific thread.
         */
        inline void Resume()
        {
            vTaskResume(GetHandle());
        }
#endif

#if (INCLUDE_xTaskResumeFromISR == 1)
        /**
         *  Resume a specific thread from ISR context.
         */
        inline void ResumeFromISR()
        {
            xTaskResumeFromISR(GetHandle());
        }
#endif

#if (INCLUDE_uxTaskPriorityGet == 1)
        /**
         *  Get the priority of this Thread.
         *
         *  @return Priority at the time this was called.
         */
        inline UBaseType_t GetPriority()
        {
            return (uxTaskPriorityGet(GetHandle()));
        }

        /**
         *  Get the priority of this Thread from ISR context.
         *
         *  @return Priority at the time this was called.
         */
        inline UBaseType_t GetPriorityFromISR()
        {
            return (uxTaskPriorityGetFromISR(GetHandle()));
        }
#endif


#if (INCLUDE_vTaskPrioritySet == 1)
        /**
         *  Set the priority of this thread.
         *
         *  @param NewPriority The thread's new priority.
         */
        inline void SetPriority(UBaseType_t NewPriority)
        {
            Priority = NewPriority;
            vTaskPrioritySet(GetHandle(), NewPriority);
        }
#endif

        /**
         *  Get the name of this thread.
         *
         *  @return a C++ string with the name of the task.
         */
#ifndef CPP_FREERTOS_NO_CPP_STRINGS
        inline std::string GetName()
        {
            return Name;
        }
#else
        inline char* GetName()
        {
            return pcTaskGetName(handle);
        }
#endif

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Available from inside your Thread implementation.
    //  You should make sure that you are only calling these methods
    //  from within your Run() method, or that your Run() method is on the
    //  callstack.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        /**
         *  Implementation of your actual thread code.
         *  You must override this function.
         *  @note If INCLUDE_vTaskDelete is defined, then you may return from
         *  your Run method. This will cause the task to be deleted from
         *  FreeRTOS, however you are still responsible to delete the
         *  task object. If this is not defined, then retuning from your Run()
         *  method will result in an assert.
         */
        virtual void Run() = 0;

#if (INCLUDE_vTaskDelete == 1)
        /**
         *  Called on exit from your Run() routine. 
         *  
         *  It is optional whether you implement this or not.
         *
         *  If you allow your Thread to exit its Run method, 
         *  implementing a Cleanup method allows you to call 
         *  your Thread's destructor. If you decide to call delete 
         *  in your Cleanup function, be aware that additional 
         *  derived classes shouldn't call delete. 
         */ 
        virtual void Cleanup();
#else
        /**
         *  If we can't delete a task, it makes no sense to have a
         *  Cleanup routine.
         */
#endif

#if (INCLUDE_vTaskDelay == 1)
        /**
         *  Delay this thread for at least Delay ticks.
         *
         *  @param Delay How long to delay the thread.
         */
        void inline Delay(const TickType_t Delay)
        {
            vTaskDelay(Delay);
        }
#endif

#if (INCLUDE_vTaskDelayUntil == 1)
        /**
         *  Delay this thread for Period ticks, taking into account
         *  the execution time of the thread.
         *
         *  This FreeRTOS permutation of delay can be used by periodic
         *  tasks to ensure a constant execution frequency.
         *
         *  @param Period How long to delay the thread.
         */
        void DelayUntil(const TickType_t Period);

        /**
         *  If you need to adjust or reset the period of the
         *  DelayUntil method.
         */
        void ResetDelayUntil();
#endif


#ifdef CPP_FREERTOS_CONDITION_VARIABLES

        /**
         *  Have this thread wait on a condition variable.
         *
         *  @note Threads wait, while ConditionVariables signal.
         *
         *  @param Cv The condition variable associated with the Wait.
         *  @param CvLock The required condition variable lock. The
         *  Lock must be held before calling Wait.
         *  @param Timeout Allows you to specify a timeout on the Wait,
         *  if desired.
         *
         *  @return true if the condition variable was signaled,
         *  false if it timed out.
         */
        bool Wait(  ConditionVariable &Cv,
                    Mutex &CvLock,
                    TickType_t Timeout = portMAX_DELAY);

#endif


    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this wrapper class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:
        /**
         *  Reference to the underlying task handle for this thread.
         *  Can be obtained from GetHandle().
         */
        TaskHandle_t handle;

        /**
         *  We need to track whether the scheduler is active or not.
         */
        static volatile bool SchedulerActive;

        /**
         *  The name of this thread.
         */
#ifndef CPP_FREERTOS_NO_CPP_STRINGS
        const std::string Name;
#else
        char Name[configMAX_TASK_NAME_LEN];
#endif

        /**
         *  Stack depth of this Thread, in words.
         */
        const uint16_t StackDepth;

        /**
         *  A saved / cached copy of what the Thread's priority is.
         */
        UBaseType_t Priority;

        /**
         *  Flag whether or not the Thread was started.
         */
        bool ThreadStarted;
        
        /**
         *  Make sure no one calls Start more than once.
         */
        static MutexStandard StartGuardLock;

        /**
         *  Adapter function that allows you to write a class
         *  specific Run() function that interfaces with FreeRTOS.
         *  Look at the implementation of the constructors and this
         *  code to see how the interface between C and C++ is performed.
         */
        static void TaskFunctionAdapter(void *pvParameters);

#if (INCLUDE_vTaskDelayUntil == 1)
        /**
         *  Flag denoting if we've setup delay until yet.
         */
        bool delayUntilInitialized;

        /**
         *  Book keeping value for delay until.
         */
        TickType_t delayUntilPreviousWakeTime;
#endif

#ifdef CPP_FREERTOS_CONDITION_VARIABLES

        /**
         *  How we wait and signal the thread when using condition variables.
         *  Because a semaphore maintains state, this solves the race
         *  condition between dropping the CvLock and waiting.
         */
        BinarySemaphore ThreadWaitSem;

        /**
         *  Internal helper function to signal this thread.
         */
        inline void Signal()
        {
            ThreadWaitSem.Give();
        }

    /**
     *  The Thread class and the ConditionVariable class are interdependent.
     *  If we allow the ConditionVariable class to access the internals of 
     *  the Thread class, we can reduce the public interface, which is a
     *  good thing.
     */
    friend class ConditionVariable;

#endif

};


}
#endif

