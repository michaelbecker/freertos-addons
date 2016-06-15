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
#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "semphr.h"
// TODO - explore replacing sprintf with stringstream
#include <cstdio>


namespace cpp_freertos {


/**
 *  This is the exception that is thrown if a Semaphore constructor fails.
 */
class SemaphoreCreateException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        SemaphoreCreateException()
        {
            sprintf(errorString, "Semaphore Constructor Failed");
        }

        /**
         *  Create the exception.
         */
        SemaphoreCreateException(const char *info)
        {
            snprintf(errorString, sizeof(errorString),
                        "Semaphore Constructor Failed %s", info);
        }

        /**
         *  Get what happened as a string.
         *  We are overriding the base implementation here.
         */
        virtual const char *what() const throw()
        {
            return errorString;
        }

    private:
        /**
         *  A text string representing what failed.
         */
        char errorString[80];
};


/**
 *
 *  Base wrapper class around FreeRTOS's implementation of semaphores.
 *
 *  It is not expected that an application will derive from this class.
 *
 *  Note that we distinguish between Semaphore, Binary Semaphores,
 *  Counting Semaphores, and Mutexes. Mutexes, while implemented as a kind
 *  of semaphore in FreeRTOS, are conceptually very different in use and
 *  behavior from semaphores. We acknowledge this difference in the class
 *  heirarchy, implementing mutextes as a completely different class heirarchy.
 */
class Semaphore {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Aquire (take) a semaphore.
         *
         *  Example of blocking indefinitely:
         *      aSemaphore.Take();
         *
         *  Example of blocking for 100 ticks:
         *      aSemaphore.Take(100);
         *
         *  @param Timeout How long to wait to get the Lock until giving up.
         *  @return true if the Semaphore was acquired, false if it timed out.
         */
        bool Take(TickType_t Timeout = portMAX_DELAY);

        /**
         *  Release (give) a semaphore.
         *
         *  @return true if the Semaphore was released, false if it failed.
         */
        bool Give();

        /**
         *  Aquire (take) a semaphore from ISR context.
         *
         *  @param pxHigherPriorityTaskWoken Did this operation result in a
         *         rescheduling event.
         *  @return true if the Semaphore was acquired, false if it timed out.
         */
        bool TakeFromISR(BaseType_t *pxHigherPriorityTaskWoken);

        /**
         *  Release (give) a semaphore from ISR context.
         *
         *  @param pxHigherPriorityTaskWoken Did this operation result in a
         *         rescheduling event.
         *  @return true if the Semaphore was released, false if it failed.
         */
        bool GiveFromISR(BaseType_t *pxHigherPriorityTaskWoken);

        /**
         *  Our destructor
         */
        virtual ~Semaphore();

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Not intended for use by application code.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        /**
         *  FreeRTOS semaphore handle.
         */
        SemaphoreHandle_t handle;

        /**
         *  We do not want a Semaphore ctor. This class should never be
         *  directly created, this is a base class only.
         */
        Semaphore();
};


/**
 *  Wrapper class for Binary Semaphores.
 */
class BinarySemaphore : public Semaphore {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor to create a binary semaphore.
         *
         *  @param set Is this semaphore "full" or not?
         *  @throws SemaphoreCreateException on failure.
         *  @return Instance of a BinarySemaphore.
         */
        BinarySemaphore(bool set = false);
};


/**
 *  Wrapper class for Counting Semaphores.
 */
class CountingSemaphore : public Semaphore {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor to create a counting semaphore.
         *  This ctor throws a SemaphoreCreateException on failure.
         *
         *  @param maxCount Must be greater than 0.
         *  @param initialCount Must not be greater than maxCount.
         *  @throws SemaphoreCreateException on failure.
         *  @return Instance of a CountingSemaphore.
         */
        CountingSemaphore(UBaseType_t maxCount, UBaseType_t initialCount);
};


}
#endif
