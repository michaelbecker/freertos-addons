#include "task.hpp"


using namespace rtos_cpp;


/**
 *
 */
Thread::Thread(   const char * const pcName,
                    uint16_t usStackDepth,
                    UBaseType_t uxPriority)
{
    if (pcName == NULL)
        pcName = "Default";

    BaseType_t rc = xTaskCreate(TaskFunctionAdapter,
                                pcName,
                                usStackDepth,
                                this,
                                uxPriority,
                                &handle);
    if (rc != pdPASS) {
        throw ThreadCreateException(rc);
    }
}


/**
 *
 */
Thread::Thread(   uint16_t usStackDepth,
                    UBaseType_t uxPriority)
{
    BaseType_t rc = xTaskCreate(TaskFunctionAdapter,
                                "Default",
                                usStackDepth,
                                this,
                                uxPriority,
                                &handle);
    if (rc != pdPASS) {
        throw ThreadCreateException(rc);
    }
}

#if 0
//
//
//

/**
 *
 */
Thread::Thread(   const char * const pcName,
                    uint16_t usStackDepth,
                    UBaseType_t uxPriority,
                    bool isPrivileged,
                    StackType_t *puxStackBuffer,
                    MemoryRegion_t (&xRegions)[ portNUM_CONFIGURABLE_REGIONS ])
{
    if (pcName == NULL)
        pcName = "Default";

    if (isPrivileged)
        uxPriority |= portPRIVILEGE_BIT;

    TaskParameters_t params =
    {
        TaskFunctionAdapter,
        pcName,
        usStackDepth,
        this,
        uxPriority,
        puxStackBuffer
    };

    params.xRegions = xRegions;

    BaseType_t rc = xTaskCreateRestricted(&params &handle);
    if (rc != pdPASS) {
        throw ThreadCreateException(rc);
    }
}


/**
 *
 */
Thread::Thread(   uint16_t usStackDepth,
                    UBaseType_t uxPriority,
                    bool isPrivileged,
                    StackType_t *puxStackBuffer,
                    MemoryRegion_t (&xRegions)[ portNUM_CONFIGURABLE_REGIONS ])
{
    if (isPrivileged)
        uxPriority |= portPRIVILEGE_BIT;

    TaskParameters_t params =
    {
        TaskFunctionAdapter,
        "Default",
        usStackDepth,
        this,
        uxPriority,
        puxStackBuffer
    };

    params.xRegions = xRegions;

    BaseType_t rc = xTaskCreateRestricted(&params &handle);
    if (rc != pdPASS) {
        throw ThreadCreateException(rc);
    }
}
#endif


#if (INCLUDE_vTaskDelete == 1)

/**
 *
 */
Thread::~Thread()
{
    vTaskDelete(handle);
    handle = -1;
}

#endif


/**
 *
 */
void Thread::TaskFunctionAdapter(void *pvParameters)
{
    Thread *thread = static_cast<Thread *>(pvParameters);

    thread->run();

#if (INCLUDE_vTaskDelete == 1)

    vTaskDelete(thread->handle);

#else
    configASSERT( ! "Cannot return from a thread.run function "
                    "if INCLUDE_vTaskDelete is not defined.");
#endif
}


#if (INCLUDE_vTaskDelayUntil == 1)
/**
 *
 */
void Thread::DelayUntil(const TickType_t Period)
{
    if (!delayUntilInitialized) {
        delayUntilInitialized = true;
        delayUntilPreviousWakeTime = xTaskGetTickCount();
    }

    vTaskDelayUntil(&delayUntilPreviousWakeTime, Period);
}


/**
 *
 */
void Thread::ResetDelayUntil()
{
    delayUntilInitialized = false;
}

#endif
