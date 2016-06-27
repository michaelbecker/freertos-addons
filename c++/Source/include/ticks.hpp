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
#ifndef TICKS_HPP_
#define TICKS_HPP_

#include "FreeRTOS.h"
#include "task.h"

#if ( configUSE_TICK_HOOK == 1 )
#include <list>
extern "C" void vApplicationTickHook();
#endif


namespace cpp_freertos {


#if ( configUSE_TICK_HOOK == 1 )

/**
 *  Declare the type of the Tick Hook functions.
 */
typedef void (*ApplicationTickHookFcn)();

#endif


/**
 *  Class encapsulating FreeRTOS idea of a tick.
 */
class Ticks {

    public:
        /**
         *  Get the current tick count.
         *
         *  @return Current tick count.
         */
        static inline TickType_t GetTicks()
        {
            return xTaskGetTickCount();
        }

        /**
         *  Get the current tick count from ISR context.
         *
         *  @return Current tick count.
         */
        static inline TickType_t GetTicksFromISR()
        {
            return xTaskGetTickCountFromISR();
        }

        /**
         *  Convert from ticks to ms.
         *
         *  @param ticks ticks to convert.
         *  @return milliseconds.
         */
        static inline TickType_t TicksToMs(TickType_t ticks)
        {
            return ticks * portTICK_PERIOD_MS;
        }

        /**
         *  Convert from ms to ticks.
         *
         *  @param milliseconds milliseconds to convert.
         *  @return ticks
         */
        static inline TickType_t MsToTicks(TickType_t milliseconds)
        {
            return milliseconds / portTICK_PERIOD_MS;
        }

        /**
         *  Convert from seconds to ticks.
         *
         *  @param milliseconds milliseconds to convert.
         *  @return ticks
         */
        static inline TickType_t SecondsToTicks(TickType_t seconds)
        {
            return (seconds * 1000) / portTICK_PERIOD_MS;
        }

#if ( configUSE_TICK_HOOK == 1 )
        /**
         *
         */
        static void RegisterTickHook(ApplicationTickHookFcn *fcn)
        {
            taskENTER_CRITICAL();

            TickHooks.push_front(fcn);

            taskEXIT_CRITICAL();
        }

        /**
         *
         */
        static void UnregisterTickHook(ApplicationTickHookFcn *fcn)
        {
            taskENTER_CRITICAL();

            TickHooks.remove(fcn);

            taskEXIT_CRITICAL();
        }
#endif

    private:

#if ( configUSE_TICK_HOOK == 1 )
        /**
         *
         */
        static std::list<ApplicationTickHookFcn *>TickHooks;

    /**
     *
     */
    friend void ::vApplicationTickHook();

#endif

};


}
#endif

