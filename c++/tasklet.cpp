#include "tasklet.hpp"


Tasklet::Tasklet()
{
}


Tasklet::~Tasklet()
{
}


void Tasklet::TaskletAdapterFunction(void *reference, uint32_t parameter)
{
    Tasklet *tasklet = static_cast<Tasklet *>(reference);

    tasklet->run(parameter);
}


bool Tasklet::ScheduleTasklet(  uint32_t parameter, 
                                TickType_t xTicksToWait)
{
    BaseType_t rc;

    rc = xTimerPendFunctionCall(TaskletAdapterFunction, 
                                this, 
                                parameter, 
                                xTicksToWait);

    return rc == pdPASS : true ? false;
}


bool Tasklet::ScheduleTaskletFromISR(   uint32_t parameter, 
                                        BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t rc;

    rc = xTimerPendFunctionCallFromISR( TaskletAdapterFunction, 
                                        this, 
                                        parameter, 
                                        pxHigherPriorityTaskWoken);

    return rc == pdPASS : true ? false;
}



