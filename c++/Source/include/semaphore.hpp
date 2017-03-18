/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
 *  
 *  Source Code:
 *  https://github.com/michaelbecker/freertos-addons
 *
 *  Project Page:
 *  http://michaelbecker.github.io/freertos-addons/
 *
 *  On-line Documentation:
 *  http://michaelbecker.github.io/freertos-addons/docs/html/index.html
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
 *  NOTE: A modification to the GPL is included to allow you to distribute a
 *  combined work that includes FreeRTOS C++ Wrappers project
 *  without being obliged to provide the source
 *  code for proprietary components.
 * 
 *  The FreeRTOS C++ Wrappers project GPL exception text follows:
 * 
 *  Any FreeRTOS C++ Wrapper *source code*, whether modified or in it's 
 *  original release form, or whether in whole or in part, can only be 
 *  distributed by you under the terms of the GNU General Public License plus
 *  this exception.  An independent module is a module which is not derived
 *  from or based on FreeRTOS C++ Wrappers project.
 * 
 *  Clause 1:
 * 
 *  Linking FreeRTOS C++ Wrappers project  with other modules is making a 
 *  combined work based on FreeRTOS C++ Wrappers project. Thus, the terms 
 *  and conditions of the GNU General Public License V2 cover the
 *  whole combination.
 * 
 *  As a special exception, the copyright holders of FreeRTOS C++ Wrappers 
 *  project give you permission to link FreeRTOS C++ Wrappers project with 
 *  independent modules to produce a statically linked executable, regardless
 *  of the license terms of these independent modules, and to copy and 
 *  distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the 
 *  terms and conditions of the license of that module.  An independent 
 *  module is a module which is not derived from or based on FreeRTOS C++ 
 *  Wrappers project.
 * 
 *  Clause 2:
 * 
 *  FreeRTOS C++ Wrappers project may not be used for any competitive or 
 *  comparative purpose, including the publication of any form of run time 
 *  or compile time metric, without the express permission of the copyright 
 *  holder(s) (this is the norm within the industry and is intended to ensure
 *  information accuracy).
 *  
 ***************************************************************************/
#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

/**
 *  C++ exceptions are used by default when constructors fail.
 *  If you do not want this behavior, define the following in your makefile
 *  or project. Note that in most / all cases when a constructor fails,
 *  it's a fatal error. In the cases when you've defined this, the new 
 *  default behavior will be to issue a configASSERT() instead.
 */
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
#include <exception>
#include <string>
#include <cstdio>
#ifdef CPP_FREERTOS_NO_CPP_STRINGS
#error "FreeRTOS-Addons require C++ Strings if you are using exceptions"
#endif
#endif
#include "FreeRTOS.h"
#include "semphr.h"


namespace cpp_freertos {


#ifndef CPP_FREERTOS_NO_EXCEPTIONS
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
        explicit SemaphoreCreateException(const char *info)
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
#endif


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
        explicit BinarySemaphore(bool set = false);
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
