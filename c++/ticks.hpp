#ifndef TICKS_HPP_
#define TICKS_HPP_

#include "FreeRTOS.h"
#include "task.h"


namespace cpp_freertos {


/**
 *  Class encapsulating FreeRTOS idea of a tick.
 */
class Ticks {

    public:
        /**
         *  Get the current tick count.
         */
        static inline TickType_t GetTicks()
        {
            return xTaskGetTickCount();
        }

        /**
         *  Get the current tick count from ISR context.
         */
        static inline TickType_t GetTicksFromISR()
        {
            return xTaskGetTickCountFromISR();
        }

        /**
         *  Convert from ticks to ms.
         */
        static inline TickType_t TicksToMs(TickType_t ticks)
        {
            return ticks * portTICK_PERIOD_MS;
        }

        /**
         *  Convert from ms to ticks.
         */
        static inline TickType_t MsToTicks(TickType_t milliseconds)
        {
            return milliseconds / portTICK_PERIOD_MS;
        }
};


}
#endif
