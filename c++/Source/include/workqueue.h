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
#ifndef WORK_QUEUE_HPP_
#define WORK_QUEUE_HPP_

#include "FreeRTOS.h"
//#include <list>

#include "thread.hpp"
#include "queue.hpp"


namespace cpp_freertos {


#define DEFAULT_MAX_WORK_ITEMS          10
#define DEFAULT_WORK_QUEUE_STACK_SIZE   (configMINIMAL_STACK_SIZE * 2)
#define DEFAULT_WORK_QUEUE_PRIORITY     (tskIDLE_PRIORITY + 1)


class WorkItem {

    public:

        WorkItem(bool autoFree = false);
        virtual ~WorkItem();

        bool GetFreeAfterRun();

    protected:
        void Run() = 0;

    private:
        bool AutoFree;
};


class WorkQueue {

    public:

        WorkQueue(  const char * const Name,
                    uint16_t StackDepth = DEFAULT_WORK_QUEUE_STACK_SIZE,
                    UBaseType_t Priority = DEFAULT_WORK_QUEUE_PRIORITY,
                    UBaseType_t maxWorkItems = DEFAULT_MAX_WORK_ITEMS);

        WorkQueue(  uint16_t StackDepth = DEFAULT_WORK_QUEUE_STACK_SIZE,
                    UBaseType_t Priority = DEFAULT_WORK_QUEUE_PRIORITY,
                    UBaseType_t maxWorkItems = DEFAULT_MAX_WORK_ITEMS);

        ~WorkQueue();

        bool QueueWork(WorkItem *work);

    private:

        class CWorkerThread : public Thread {

            public:
                CWorkerThread(  const char * const Name,
                                uint16_t StackDepth,
                                UBaseType_t Priority);

                CWorkerThread(  uint16_t StackDepth,
                                UBaseType_t Priority);

            protected:
                void Run();
        };

        CWorkerThread *WorkerThread;
        Queue *WorkItemQueue;
};


}
#endif


