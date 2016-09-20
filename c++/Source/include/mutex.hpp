/****************************************************************************
 *
 *  Copyright (c) 2016, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C++ Wrappers project.
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
 ***************************************************************************/
#ifndef MUTEX_HPP_
#define MUTEX_HPP_

#ifndef CPP_FREERTOS_NO_EXCEPTIONS
#include <exception>
#include <string>
// TODO - explore replacing sprintf with stringstream
#include <cstdio>
#endif
#include "FreeRTOS.h"
#include "semphr.h"

namespace cpp_freertos {


#ifndef CPP_FREERTOS_NO_EXCEPTIONS
/**
 *  This is the exception that is thrown if a Mutex constructor fails.
 */
class MutexCreateException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        MutexCreateException()
        {
            sprintf(errorString, "Mutex Constructor Failed");
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
 *  Base wrapper class around FreeRTOS's implementation of mutexes.
 *
 *  By definition, Mutexes can @em NOT be used from ISR contexts.
 *
 *  @note It is expected that an application will instantiate one of the
 *        derived classes and use that object for synchronization. It is
 *        not expected that a user or application will derive from these
 *        classes.
 */
class Mutex {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Lock the Mutex.
         *
         *  Each type of Mutex implements it's own locking code as per the
         *  FreeRTOS API.
         *
         *  @param Timeout How long to wait to get the Lock until giving up.
         *  @return true if the Lock was acquired, false if it timed out.
         */
        virtual bool Lock(TickType_t Timeout = portMAX_DELAY) = 0;

        /**
         *  Unlock the Mutex.
         *
         *  @return true if the Lock was released, false if it failed. (Hint,
         *           if it fails, did you call Lock() first?)
         */
        virtual bool Unlock() = 0;

        /**
         *  Our destructor
         */
        virtual ~Mutex();

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
         *  This constructor should not be public.
         */
        Mutex();
};


/**
 *  Standard usage Mutex.
 *  By default calls to Lock these objects block forever, but this can be
 *  changed by simply passing in a argument to the Lock() method.
 *  These objects are not recursively acquirable. Calling Lock() twice from
 *  the same Thread (i.e. task) will deadlock.
 *
 *  @note Standard mutexes use less resources than recursive mutexes. You
 *        should typically use this type of Mutex, unless you have a strong
 *        need for a MutexRecursive mutex.
 */
class MutexStandard : public Mutex {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Create a standard, non-recursize Mutex.
         *
         *  @throws ThreadMutexException on failure.
         */
        MutexStandard();

        /**
         *  Lock the Mutex.
         *
         *  @param Timeout How long to wait to get the Lock until giving up.
         *  @return true if the Lock was acquired, false if it timed out.
         */
        virtual bool Lock(TickType_t Timeout = portMAX_DELAY);

        /**
         *  Unlock the Mutex.
         *
         *  @return true if the Lock was released, false if it failed. (Hint,
         *           if it fails, did you call Lock() first?)
         */
        virtual bool Unlock();
};


#if (configUSE_RECURSIVE_MUTEXES == 1)

/**
 *  Recursive usage Mutex.
 *
 *  By default calls to Lock these objects block forever, but this can be
 *  changed by simply passing in a argument to the Lock() method.
 *  These objects are recursively acquirable. Calling Lock() twice from
 *  the same Thread (i.e. task) works fine. The caller just needs to be sure to
 *  call Unlock() as many times as Lock().
 *
 *  @note Recursive mutexes use more resources than standard mutexes. You
 *        should be sure that you actually need this type of synchronization
 *        before using it.
 */
class MutexRecursive : public Mutex {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Create a recursize Mutex.
         *
         *  @throws ThreadMutexException on failure.
         */
        MutexRecursive();

        /**
         *  Lock the Mutex.
         *
         *  @param Timeout How long to wait to get the Lock until giving up.
         *  @return true if the Lock was acquired, false if it timed out.
         */
        virtual bool Lock(TickType_t Timeout = portMAX_DELAY);

        /**
         *  Unlock the Mutex.
         *
         *  @return true if the Lock was released, false if it failed. (Hint,
         *           if it fails, did you call Lock() first?)
         */
        virtual bool Unlock();
};

#endif


/**
 *  Synchronization helper class that leverages the C++ language to help
 *  prevent deadlocks.
 *  This is a C++11 feature that allows Mutex Locking and Unlocking to behave
 *  following an RAII style. The constructor of this helper object locks the
 *  Mutex. The destructor unlocks the Mutex. Since C++ guarantees that an
 *  object's desctuctor is always called when it goes out of scope, calls to
 *  Unlock become unnecessary and are in fact guaranteed as long as correct
 *  scoping is used.
 */
class LockGuard {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Create a LockGuard with a specific Mutex.
         *
         *  @post The Mutex will be locked.
         *  @note There is an infinite timeout for acquiring the Lock.
         */
        explicit LockGuard(Mutex& m);

        /**
         *  Destroy a LockGuard.
         *
         *  @post The Mutex will be unlocked.
         */
        ~LockGuard();

        /////////////////////////////////////////////////////////////////////////
        //
        //  Private API
        //
        /////////////////////////////////////////////////////////////////////////
    private:
        /**
         *  We do not want a copy constructor.
         */
        LockGuard(const LockGuard&);

        /**
         *  Reference to the Mutex we locked, so it can be unlocked
         *  in the destructor.
         */
        Mutex& mutex;
};


}
#endif
