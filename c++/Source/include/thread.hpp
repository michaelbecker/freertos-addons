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
#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "task.h"
// TODO - explore replacing sprintf with stringstream
#include <cstdio>


namespace cpp_freertos {


/**
 *  This is the exception that is thrown if a Thread constructor fails.
 */
class ThreadCreateException  : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        explicit ThreadCreateException(BaseType_t error)
        {
            errorCode = error;
            sprintf(errorString, "Thread Constructor Failed %d", (int)errorCode);
        }

        /**
         *  Get what happened as a string.
         *  We are overriding the base implementation here.
         */
        virtual const char *what() const throw()
        {
            return errorString;
        }

        /**
         *  Get the FreeRTOS error code.
         */
        BaseType_t getErrorCode()
        {
            return errorCode;
        }

    private:
        /**
         *  Save the FreeRTOS return code code from xTaskCreate().
         */
        BaseType_t errorCode;

        /**
         *  A text string representing what failed.
         */
        char errorString[40];
};


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
         *  @throws ThreadCreateException on failure.
         *  @param Name Name of the thread. Only useful for debugging.
         *  @param StackDepth Number of "words" allocated for the Thread stack.
         *  @param Priority FreeRTOS priority of this Thread.
         */
        Thread( const char * const Name,
                uint16_t StackDepth,
                UBaseType_t Priority);

        /**
         *  Constructor to create an unnamed thread.
         *
         *  @throws ThreadCreateException on failure.
         *  @param StackDepth Number of "words" allocated for the Thread stack.
         *  @param Priority FreeRTOS priority of this Thread.
         */
        Thread( uint16_t StackDepth,
                UBaseType_t Priority);


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
         */
        static inline void StartScheduler()
        {
            vTaskStartScheduler();
        }

        /**
         *  End the scheduler.
         *
         *  @note Please see the FreeRTOS documentation regarding constraints
         *  with the implementation of this.
         */
        static inline void EndScheduler()
        {
            vTaskEndScheduler();
        }

#if (INCLUDE_vTaskSuspend == 1)
        /**
         *  Suspend a specific thread.
         */
        static inline void Suspend(Thread& thread)
        {
            vTaskSuspend(thread.GetHandle());
        }

        /**
         *  Resume a specific thread.
         *
         *  @param thread Reference to the thread to resume.
         */
        static inline void Resume(Thread& thread)
        {
            vTaskResume(thread.GetHandle());
        }
#endif

#if (INCLUDE_xTaskResumeFromISR == 1)
        /**
         *  Resume a specific thread from ISR context.
         *
         *  @param thread Reference to the thread to resume.
         */
        static inline void ResumeFromISR(Thread& thread)
        {
            xTaskResumeFromISR(thread.GetHandle());
        }
#endif

#if (INCLUDE_uxTaskPriorityGet == 1)
        /**
         *  Get the priority of another thread.
         *
         *  @param thread Reference to the thread.
         *  @return Priority at the time this was called.
         */
        static inline UBaseType_t GetPriority(Thread& thread)
        {
            return (uxTaskPriorityGet(thread.GetHandle()));
        }

        /**
         *  Get the priority of another thread from ISR context.
         *
         *  @param thread Reference to the thread.
         *  @return Priority at the time this was called.
         */
        static inline UBaseType_t GetPriorityFromISR(Thread& thread)
        {
            return (uxTaskPriorityGetFromISR(thread.GetHandle()));
        }
#endif

#if (INCLUDE_vTaskPrioritySet == 1)
        /**
         *  Set the priority of another thread.
         *
         *  @param thread Reference to the thread.
         *  @param NewPriority The thread's new priority.
         */
        static inline void SetPriority( Thread& thread,
                                        UBaseType_t NewPriority)
        {
            vTaskPrioritySet(thread.GetHandle(), NewPriority);
        }
#endif

#if (INCLUDE_pcTaskGetTaskName == 1)
        /**
         *  Get the name of a thread.
         *
         *  @param thread Reference to the thread.
         *  @return The thread's name.
         */
        static inline std::string Name(Thread& thread)
        {
            std::string name = pcTaskGetTaskName(thread.GetHandle());
            return name;
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

#if ( INCLUDE_vTaskSuspend == 1 )
        /**
         *  Suspend this thread.
         *
         *  @note There is no Resume() function, becaue you
         *  can't resume from the thread you just suspended, because
         *  it's suspended.
         */
        void inline Suspend()
        {
            vTaskSuspend(NULL);
        }
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

#if (INCLUDE_uxTaskPriorityGet == 1)
        /**
         *  Obtain our own priority.
         *
         *  @return Our priority.
         */
        UBaseType_t inline GetPriority()
        {
            return (uxTaskPriorityGet(NULL));
        }

        /**
         *  Obtain our own priority from an ISR context.
         *
         *  @return Our priority.
         */
        UBaseType_t inline GetPriorityFromISR()
        {
            return (uxTaskPriorityGetFromISR(NULL));
        }

#endif

#if (INCLUDE_vTaskPrioritySet == 1)
        /**
         *  Change the priority of this thread.
         *
         *  @param NewPriority The thread's new priority.
         */
        void SetPriority(UBaseType_t NewPriority)
        {
            vTaskPrioritySet(NULL, NewPriority);
        }
#endif

#if (INCLUDE_pcTaskGetTaskName == 1)
        /**
         *  Get the name of this thread.
         *
         *  @return The thread's name.
         */
        static inline std::string Name()
        {
            std::string name = pcTaskGetTaskName(NULL);
            return name;
        }
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
