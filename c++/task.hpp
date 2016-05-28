/****************************************************************************
 *
 *  @Note
 *  + Features not implemented yet
 *    - MPU restricted threads 
 *    - eTaskGetState
 *    - uxTaskGetNumberOfTasks
 *    - uxTaskGetStackHighWaterMark
 *    - Thread Local Storage
 *
 ***************************************************************************/
#ifndef TASK_HPP_
#define TASK_HPP_

#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "task.h"


/**
 *  This is the exception that is thrown if a CThread constructor fails.
 */
class CThreadCreationException  : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        CThreadCreationException(BaseType_t error)
        {
            errorCode = error;
            sprintf(errorString, "CThread Constructor Failed %d", errorCode);
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
 */
class CThread {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //  Available from anywhere. Many of these require a CThread reference 
    //  if they are operating on a thread.
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor to create a named thread.
         *  This ctor throws a CThreadCreationException on failure.
         */
        CThread(    const char * const pcName,
                    uint16_t usStackDepth,
                    UBaseType_t uxPriority);

        /**
         *  Constructor to create an unnamed thread.
         *  This ctor throws a CThreadCreationException on failure.
         */
        CThread(    uint16_t usStackDepth,
                    UBaseType_t uxPriority);

        /**
         *  Constructor to create an MPU restricted named thread.
         *  This ctor throws a CThreadCreationException on failure.
         */
        CThread(    const char * const pcName,
                    uint16_t usStackDepth,
                    UBaseType_t uxPriority,
                    bool isPrivileged,
                    StackType_t *puxStackBuffer);

        /**
         *  Constructor to create an MPU restricted unnamed thread.
         *  This ctor throws a CThreadCreationException on failure.
         */
        CThread(    uint16_t usStackDepth,
                    UBaseType_t uxPriority,
                    bool isPrivileged,
                    StackType_t *puxStackBuffer);

#if (INCLUDE_vTaskDelete == 1)
        /**
         *  Our destructor.
         */
        virtual ~CThread();
#else
        /**
         *  If we can't delete a task, it makes no sense to have a 
         *  destructor.
         */
        ~CThread() = delete;
#endif

        /**
         *  Accessor to get the thread's backing task handle.
         *  There is no setter, on purpose.
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
        static inline void Suspend(CThread& thread)
        {
            vTaskSuspend(thread.GetHandle());
        }

        /**
         *  Resume a specific thread.
         */
        static inline void Resume(CThread& thread)
        {
            vTaskResume(thread.GetHandle());
        }
#endif        

#if (INCLUDE_xTaskResumeFromISR == 1)
        /**
         *  Resume a specific thread from ISR context.
         */
        static inline void ResumeFromISR(CThread& thread)
        {
            xTaskResumeFromISR(thread.GetHandle());
        }
#endif

#if (INCLUDE_uxTaskPriorityGet == 1)
        /**
         *  Get the priority of another thread.
         */
        static inline UBaseType_t GetPriority(CThread& thread)
        {
            return (uxTaskPriorityGet(thread.GetHandle()));
        }

        /**
         *  Get the priority of another thread from ISR context.
         */
        static inline UBaseType_t GetPriorityFromISR(CThread& thread)
        {
            return (uxTaskPriorityGetFromISR(thread.GetHandle()));
        }
#endif

#if (INCLUDE_vTaskPrioritySet == 1)
        /**
         *  Get the priority of another thread.
         */
        static inline void SetPriority( CThread& thread, 
                                        UBaseType_t uxNewPriority);
        {
            vTaskPrioritySet(thread.GetHandle(), uxNewPriority);
        }
#endif

#if (INCLUDE_pcTaskGetTaskName == 1)
        static inline std::string Name(CThread& thread)
        {
            std::string name = pcTaskGetTaskName(thread.GetHandle());
            return name;
        }
#endif


    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Available from inside your CThread implementation. 
    //  You should make sure that you are only calling these methods 
    //  from within your Run() method, or at Run() method is on the 
    //  callstack.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        /**
         *  Implementation of your actual thread code.
         */
        virtual void Run() = 0;

#if ( INCLUDE_vTaskSuspend == 1 )
        /**
         *  Suspend this thread.
         *  Note that there is no Resume() function, becaue you 
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
         *  Delay this thread for at least xTicksToDelay ticks.
         */
        void inline Delay(const TickType_t xTicksToDelay)
        {
            vTaskDelay(xTicksToDelay);
        }
#endif

#if (INCLUDE_vTaskDelayUntil == 1)
        /**
         *  Delay this thread for at least xTicksToDelay ticks.
         *  This FreeRTOS permutation of delay can be used by periodic
         *  tasks to ensure a constant execution frequency.
         */
        void DelayUntil(const TickType_t Period);

        /**
         *  If you need to adjust or reset the period.
         */
        void ResetDelayUntil();
#endif 

#if (INCLUDE_uxTaskPriorityGet == 1)
        /**
         *  Obtain our own priority.
         */        
        UBaseType_t inline GetPriority()
        {
            return (uxTaskPriorityGet(NULL));
        }

        /**
         *  Obtain our own priority from an ISR context.
         */        
        UBaseType_t inline GetPriorityFromISR()
        {
            return (uxTaskPriorityGetFromISR(NULL));
        }
    
#endif

#if (INCLUDE_vTaskPrioritySet == 1)
        /**
         *  Change the priority of this thread.
         */
        void SetPriority(UBaseType_t uxNewPriority)
        {
            vTaskPrioritySet(NULL, uxNewPriority);
        }   
#endif

#if (INCLUDE_pcTaskGetTaskName == 1)
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
         */
        static void TaskFunctionAdapter(void *pvParameters);

#if (INCLUDE_vTaskDelayUntil == 1)
        /**
         *  Flag denoting if we've setup delay until yet.
         */
        bool delayUntilInitialized {false};

        /**
         *  Book keeping value for delay until.
         */
        TickType_t delayUntilPreviousWakeTime;
#endif 

};


#endif

