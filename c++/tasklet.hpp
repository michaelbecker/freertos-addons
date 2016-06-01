#ifndef TASKLET_HPP_
#define TASKLET_HPP_


#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "timers.h"


namespace cpp_freertos {


class Tasklet {

    public:

        Tasklet();
        virtual ~Tasklet();

        bool ScheduleTasklet(   uint32_t parameter,
                                TickType_t xTicksToWait = portMAX_DELAY);

        bool ScheduleTaskletFromISR(uint32_t parameter,
                                    BaseType_t *pxHigherPriorityTaskWoken);

    protected:
        virtual void Run(uint32_t parameter) = 0;

    private:
        void TaskletAdapterFunction(void *ref, uint32_t parameter);

};

}
#endif
