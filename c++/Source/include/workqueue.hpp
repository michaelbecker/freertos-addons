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
#ifndef WORK_QUEUE_HPP_
#define WORK_QUEUE_HPP_

#include "thread.hpp"
#include "queue.hpp"
#include "semaphore.hpp"


namespace cpp_freertos {


#define DEFAULT_MAX_WORK_ITEMS          10
#define DEFAULT_WORK_QUEUE_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
#define DEFAULT_WORK_QUEUE_PRIORITY     (tskIDLE_PRIORITY + 1)


/**
 *  This class encapsulates the idea of a discrete, non-repeating task.
 *  Create a WorkItem when there is something you need to do on a different
 *  Thread, but doesn't have to happen periodically. This is a great 
 *  construct for one off fire and forget tasks.
 *
 *  This is an abstract base class.
 *  To use this, you need to subclass it. All of your WorkItems should
 *  be derived from this class. Then implement the virtual Run
 *  function. This is a similar design to Java threading.
 */
class WorkItem {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:

        /**
         *  Our constructor.
         *
         *  @param freeAfterComplete If you pass in a true, you are 
         *  requesing the WorkQueue itself to delete this WorkItem after
         *  it has run it. 
         *  @note Only set freeAfterComplete = true if:
         *  1) You dynamically allocated it (i.e. used "new")
         *  2) After you call QueueWork() you promise never to touch 
         *     this object again. 
         */
        WorkItem(bool freeAfterComplete = false);

        /**
         *  Our destructor.
         */
        virtual ~WorkItem();
        
        /**
         *  Allows a client to decide if this WorkItem is marked
         *  for automatic deletion.
         */
        bool FreeAfterRun();

        /**
         *  Implementation of your actual WorkItem function.
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
         *  Designates whether this WorkItem should be deleted 
         *  after the WorkQueue has run it.
         */
        const bool FreeItemAfterCompleted;
};


/**
 *  This class is the "engine" for WorkItems. Create one or more WorkQueues
 *  to accept WorkItems. WorkQueues pull WorkItems off of a FIFO queue and 
 *  run them sequentially.
 */
class WorkQueue {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor to create a named WorkQueue.
         *
         *  @throws ThreadCreateException, QueueCreateException, 
         *          SemaphoreCreateException
         *  @param Name Name of the thread internal to the WorkQueue. 
         *         Only useful for debugging.
         *  @param StackDepth Number of "words" allocated for the Thread stack.
         *  @param Priority FreeRTOS priority of this Thread.
         *  @param MaxWorkItems Maximum number of WorkItems this WorkQueue can hold.
         */
        WorkQueue(  const char * const Name,
                    uint16_t StackDepth = DEFAULT_WORK_QUEUE_STACK_SIZE,
                    UBaseType_t Priority = DEFAULT_WORK_QUEUE_PRIORITY,
                    UBaseType_t MaxWorkItems = DEFAULT_MAX_WORK_ITEMS);

        /**
         *  Constructor to create an unnamed WorkQueue.
         *
         *  @throws ThreadCreateException, QueueCreateException, 
         *          SemaphoreCreateException
         *  @param StackDepth Number of "words" allocated for the Thread stack.
         *  @param Priority FreeRTOS priority of this Thread.
         *  @param MaxWorkItems Maximum number of WorkItems this WorkQueue can hold.
         */
        WorkQueue(  uint16_t StackDepth = DEFAULT_WORK_QUEUE_STACK_SIZE,
                    UBaseType_t Priority = DEFAULT_WORK_QUEUE_PRIORITY,
                    UBaseType_t MaxWorkItems = DEFAULT_MAX_WORK_ITEMS);

        /**
         *  Our destructor.
         *
         *  @note Given the multithreaded nature of this class, the dtor 
         *  may block until the underlying Thread has had a chance to 
         *  clean up.
         */
        ~WorkQueue();

        /**
         *  Send a WorkItem off to be executed.
         *
         *  @param work Pointer to a WorkItem.
         *  @return true if it was queued, false otherwise.
         *  @note This function may block if the WorkQueue is presently full.
         */ 
        bool QueueWork(WorkItem *work);

    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:

        /**
         *  An internal derived Thread class, in which we do our real work.
         */
        class CWorkerThread : public Thread {

            public:
                CWorkerThread(  const char * const Name,
                                uint16_t StackDepth,
                                UBaseType_t Priority,
                                WorkQueue *Parent);

                CWorkerThread(  uint16_t StackDepth,
                                UBaseType_t Priority,
                                WorkQueue *Parent);

                virtual ~CWorkerThread();

            protected:
                virtual void Run();

            private:
                const WorkQueue *ParentWorkQueue;
        };
        
        /**
         *  Pointer to our WorkerThread.
         */
        CWorkerThread *WorkerThread;

        /**
         *  Pointer to our work queue itself.
         */
        Queue *WorkItemQueue;

        /**
         *  Semaphore to support deconstruction without race conditions.
         */
        BinarySemaphore *ThreadComplete;
};


}
#endif


