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
#include "workqueue.hpp"


using namespace cpp_freertos;


WorkItem::WorkItem(bool autoFree)
{
    AutoFree = autoFree;
}


WorkItem::~WorkItem()
{
}


bool WorkItem::FreeAfterRun()
{
    return AutoFree;
}


WorkQueue::WorkQueue(   const char * const Name,
                        uint16_t StackDepth,
                        UBaseType_t Priority,
                        UBaseType_t maxWorkItems)
{
    WorkerThread = new CWorkerThread(Name, StackDepth, Priority);
    WorkItemQueue = new Queue(maxWorkItems, sizeof(WorkItem *));
}


WorkQueue::WorkQueue(   uint16_t StackDepth,
                        UBaseType_t Priority,
                        UBaseType_t maxWorkItems)
{
    WorkerThread = new CWorkerThread(StackDepth, Priority);
    WorkItemQueue = new Queue(maxWorkItems, sizeof(WorkItem *));
}


WorkQueue::~WorkQueue()
{
    WorkItemQueue->Flush();
    WorkItemQueue->Enqueue(NULL);
    delete WorkItemQueue;
}


bool WorkQueue::QueueWork(WorkItem *work)
{
    return WorkItemQueue->Enqueue(work);
}


WorkQueue::CWorkerThread::CWorkerThread(const char * const Name,
                                        uint16_t StackDepth,
                                        UBaseType_t Priority)
    : Thread(Name, StackDepth, Priority)
{
}


WorkQueue::CWorkerThread::CWorkerThread(uint16_t StackDepth,
                                        UBaseType_t Priority)
    : Thread(StackDepth, Priority)
{
}


void WorkQueue::CWorkerThread::Run()
{
    while (true) {

        WorkItem *work;

        bool success = WorkItemQueue.Dequeue(&work);

        if (success && !work) {
            break;
        }

        work->Run();
        if (work->FreeAfterRun()) {
            delete work;
        }
    }
}


