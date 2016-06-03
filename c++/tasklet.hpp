#ifndef TASKLET_HPP_
#define TASKLET_HPP_


#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "timers.h"


namespace cpp_freertos {


class Tasklet {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        Tasklet();
        virtual ~Tasklet();

        bool ScheduleTasklet(   uint32_t parameter,
                                TickType_t xTicksToWait = portMAX_DELAY);

        bool ScheduleTaskletFromISR(uint32_t parameter,
                                    BaseType_t *pxHigherPriorityTaskWoken);

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
        virtual void Run(uint32_t parameter) = 0;

    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this wrapper class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:
        void TaskletAdapterFunction(void *ref, uint32_t parameter);

};

}
#endif
