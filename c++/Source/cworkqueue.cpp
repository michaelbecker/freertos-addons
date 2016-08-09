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
#include "workqueue.hpp"


using namespace cpp_freertos;


WorkItem::WorkItem(bool freeAfterComplete)
    : FreeItemAfterCompleted(freeAfterComplete)
{
}


WorkItem::~WorkItem()
{
}


bool WorkItem::FreeAfterRun()
{
    return FreeItemAfterCompleted;
}


WorkQueue::WorkQueue(   const char * const Name,
                        uint16_t StackDepth,
                        UBaseType_t Priority,
                        UBaseType_t maxWorkItems)
{
    //
    //  Build the Queue first, since the Thread is going to access 
    //  it as soon as it can, maybe before we leave this ctor.
    //
    WorkItemQueue = new Queue(maxWorkItems, sizeof(WorkItem *));
    ThreadComplete = new BinarySemaphore();
    WorkerThread = new CWorkerThread(Name, StackDepth, Priority, this);
    //
    //  Our ctor chain is complete, we can start.
    //
    WorkerThread->Start();
}


WorkQueue::WorkQueue(   uint16_t StackDepth,
                        UBaseType_t Priority,
                        UBaseType_t maxWorkItems)
{
    //
    //  Build the Queue first, since the Thread is going to access 
    //  it as soon as it can, maybe before we leave this ctor.
    //
    WorkItemQueue = new Queue(maxWorkItems, sizeof(WorkItem *));
    ThreadComplete = new BinarySemaphore();
    WorkerThread = new CWorkerThread(StackDepth, Priority, this);
    //
    //  Our ctor chain is complete, we can start.
    //
    WorkerThread->Start();
}


WorkQueue::~WorkQueue()
{
    //
    //  This dtor is tricky, because of the multiple objects in 
    //  play, and the multithreaded nature of this specific object.
    //

    //
    //  Note that we cannot flush the queue. If there are items 
    //  in the queue maked freeAfterComplete, we would leak the 
    //  memory. 
    //

    //
    //  Send a message that it's time to cleanup.
    //
    WorkItem *work = NULL;
    WorkItemQueue->Enqueue(&work);

    //
    //  Wait until the thread has run enough to signal that it's done.
    //
    ThreadComplete->Take();

    //
    //  Then delete the queue and thread. Order doesn't matter here.
    //
    delete WorkItemQueue;
    delete WorkerThread;
    delete ThreadComplete;
}


bool WorkQueue::QueueWork(WorkItem *work)
{
    return WorkItemQueue->Enqueue(&work);
}


WorkQueue::CWorkerThread::CWorkerThread(const char * const Name,
                                        uint16_t StackDepth,
                                        UBaseType_t Priority,
                                        WorkQueue *Parent)
    : Thread(Name, StackDepth, Priority), ParentWorkQueue(Parent)
{
}


WorkQueue::CWorkerThread::CWorkerThread(uint16_t StackDepth,
                                        UBaseType_t Priority,
                                        WorkQueue *Parent)
    : Thread(StackDepth, Priority), ParentWorkQueue(Parent)
{
}


WorkQueue::CWorkerThread::~CWorkerThread()
{
}


void WorkQueue::CWorkerThread::Run()
{
    while (true) {

        WorkItem *work;

        //
        //  Wait forever for work.
        //
        ParentWorkQueue->WorkItemQueue->Dequeue(&work);

        //
        //  If we dequeue a NULL item, its our sign to exit.
        //  We are being deconstructed.
        //
        if (work == NULL) {
            //
            //  Exit the task loop.
            //
            break;
        }

        //
        //  Else we have an item, run it.
        //
        work->Run();

        //
        //  If this was a dynamic, fire and forget item and we were 
        //  requested to clean it up, do so.
        //
        if (work->FreeAfterRun()) {
            delete work;
        }
    }

    //
    //  Signal the dtor that the thread is exiting.
    //
    ParentWorkQueue->ThreadComplete->Give();
}


