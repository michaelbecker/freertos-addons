/****************************************************************************
 *
 ***************************************************************************/
#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "semphr.h"


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
         */
        bool Take(TickType_t xBlockTime = portMAX_DELAY);

        /**
         *  Release (give) a semaphore.
         */
        bool Give();

        /**
         *  Aquire (take) a semaphore from ISR context.
         */
        bool TakeFromISR(BaseType_t *pxHigherPriorityTaskWoken);

        /**
         *  Release (give) a semaphore from ISR context.
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
        SemaphoreHandle_t handle;

    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this wrapper class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:
        /**
         *  We do not want a Semaphore ctor. This class should never be
         *  directly created, this is a base class only.
         */
        Semaphore() = delete;
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
         *  @returns Instance of a BinarySemaphore.
         *  @set Is this semaphore "full" or not?
         *  @throws SemaphoreCreateException on failure.
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
         *  @returns Instance of a CountingSemaphore.
         *  @param maxCount Must be greater than 0.
         *  @param initialCount Must not be greater than maxCount.
         *  @throws SemaphoreCreateException on failure.
         */
        CountingSemaphore(UBaseType_t maxCount, UBaseType_t initialCount);
};


}
#endif
