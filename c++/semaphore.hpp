/****************************************************************************
 *
 ***************************************************************************/
#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "semphr.h"


/**
 *  This is the exception that is thrown if a CSemaphore constructor fails.
 */
class CSemaphoreCreationException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        CSemaphoreCreationException()
        {
            sprintf(errorString, "CSemaphore Constructor Failed");
        }

        /**
         *  Create the exception.
         */
        CSemaphoreCreationException(const char *info)
        {
            snprintf(errorString, sizeof(errorString), 
                        "CSemaphore Constructor Failed %s", info);
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
class CSemaphore {

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
        virtual ~CSemaphore();

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
         *  We do not want a CSemaphore ctor. This class should never be 
         *  directly created, this is a base class only.
         */
        CSemaphore() = delete;
};


/**
 *  Wrapper class for Binary Semaphores.
 */
class CBinarySemaphore : public CSemaphore {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor to create a binary semaphore.
         *
         *  @returns Instance of a CBinarySemaphore.
         *  @set Is this semaphore "full" or not?
         *  @throws CSemaphoreCreationException on failure.
         */
        CBinarySemaphore(bool set = false);
};


/**
 *  Wrapper class for Counting Semaphores.
 */
class CCountingSemaphore : public CSemaphore {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor to create a counting semaphore.
         *  This ctor throws a CSemaphoreCreationException on failure.
         *
         *  @returns Instance of a CCountingSemaphore.
         *  @param maxCount Must be greater than 0.
         *  @param initialCount Must not be greater than maxCount.
         *  @throws CSemaphoreCreationException on failure.
         */
        CCountingSemaphore(UBaseType_t maxCount, UBaseType_t initialCount);
};


#endif

