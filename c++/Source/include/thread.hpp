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
#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "task.h"
#include "mutex.hpp"

namespace cpp_freertos {


/**
 *  Wrapper class around FreeRTOS's implementation of a task.
 *
 *  This is an abstract base class.
 *  To use this, you need to subclass it. All of your threads should
 *  be derived from the Thread class. Then implement the virtual Run
 *  function. This is a similar design to Java threading.
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
        Thread( const std::string Name,
                uint16_t StackDepth,
                UBaseType_t Priority);

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

#if (INCLUDE_vTaskDelete == 1)
        /**
         *  Our destructor, if FreeRTOS is configured to allow task deletion.
         */
        virtual ~Thread();
#else
        /**
         *  If we can't delete a task, it makes no sense to have a
         *  destructor.
         */
        ~Thread() = delete;
#endif

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
         *
         *  @param thread Reference to the thread to resume.
         */
        inline void Resume()
        {
            vTaskResume(GetHandle());
        }
#endif

#if (INCLUDE_xTaskResumeFromISR == 1)
        /**
         *  Resume a specific thread from ISR context.
         *
         *  @param thread Reference to the thread to resume.
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
        inline std::string GetName()
        {
            return Name;
        }


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
        const std::string Name;

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

};


}
#endif
