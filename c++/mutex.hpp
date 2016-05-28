#ifndef MUTEX_HPP_
#define MUTEX_HPP_

#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "semphr.h"


/**
 *  This is the exception that is thrown if a CSemaphore constructor fails.
 */
class CMutexCreationException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        CMutexCreationException()
        {
            sprintf(errorString, "CMutex Constructor Failed");
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
 *  Base wrapper class around FreeRTOS's implementation of mutexes.
 *
 *  It is not expected that an application will derive from this class.
 *
 *  Mutexes can not be used from ISR contexts.
 *
 */
class CMutex {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        bool Lock(TickType_t xBlockTime) = 0;
        bool Unlock() = 0;

        /**
         *  Our destructor
         */
        virtual ~CMutex();

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
        CMutex() = delete;

};


class CMutexStandard {
    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        CMutexStandard();
        bool Lock(TickType_t xBlockTime = portMAX_DELAY);
        bool Unlock();        
};


class CMutexRecursive {
    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        CMutexRecursive();
        bool Lock(TickType_t xBlockTime = portMAX_DELAY);
        bool Unlock();
};



class CLockGuard {
    public:
        explicit CLockGuard(CMutex& m);
        ~CLockGuard();

    private:
        CLockGuard(const CLockGuard&) = delete;
        CMutex& mutex;
};


#endif

