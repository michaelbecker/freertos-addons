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
#ifndef READ_WRITE_LOCK_HPP_
#define READ_WRITE_LOCK_HPP_


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
 *  This is the exception that is thrown if a ReadWriteLock constructor fails.
 */
class ReadWriteLockCreateException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        ReadWriteLockCreateException()
        {
            sprintf(errorString, "ReadWriteLock Constructor Failed");
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
 *  Abstract base class encapsulating a Reader/Writer lock.
 *
 *  These locks are based on mutexs and cannot be used in any way from
 *  ISR context. Likewise, these locks block indefinitely.
 *
 *  @note It is expected that an application will instantiate one of the
 *        derived classes and use that object for synchronization. It is
 *        not expected that a user or application will derive from these
 *        classes.
 */
class ReadWriteLock {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Constructor
         *
         *  @throws ReadWriteLockCreateException on failure.
         */
        ReadWriteLock();

        /**
         *  Destructor
         */
        virtual ~ReadWriteLock();

        /**
         *  Take the lock as a Reader.
         *  This allows multiple reader access.
         */
        virtual void ReaderLock() = 0;

        /**
         *  Unlock the Reader.
         */
        virtual void ReaderUnlock() = 0;

        /**
         *  Take the lock as a Writer.
         *  This allows only one thread access.
         */
        virtual void WriterLock() = 0;

        /**
         *  Unlock the Writer.
         */
        virtual void WriterUnlock() = 0;

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Not intended for use by application code.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        /**
         *  How many active readers are there.
         */
        int ReadCount;

        /**
         *  Protect ReadCount.
         */
        SemaphoreHandle_t ReadLock;

        /**
         *  Protect this resource from multiple writer access, or
         *  from Reader access when a writer is changing something.
         */
        SemaphoreHandle_t ResourceLock;
};


/**
 *  Concrete derived class that implements a Reader/Writer lock
 *  that favors the Readers. That is, with enough aggressive readers,
 *  a Writer may starve.
 */
class ReadWriteLockPreferReader : public ReadWriteLock {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Take the lock as a Reader.
         *  This allows multiple reader access.
         */
        virtual void ReaderLock();

        /**
         *  Unlock the Reader.
         */
        virtual void ReaderUnlock();

        /**
         *  Take the lock as a Writer.
         *  This allows only one thread access.
         */
        virtual void WriterLock();

        /**
         *  Unlock the Writer.
         */
        virtual void WriterUnlock();
};


/**
 *  Concrete derived class that implements a Reader/Writer lock
 *  that favors the Writers. That is, with enough aggressive writers,
 *  a Reader may starve.
 */
class ReadWriteLockPreferWriter : public ReadWriteLock {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        /**
         *  Our derived constructor.
         */
        ReadWriteLockPreferWriter();

        /**
         *  Our derived destructor.
         */
        virtual ~ReadWriteLockPreferWriter();

        /**
         *  Take the lock as a Reader.
         *  This allows multiple reader access.
         */
        virtual void ReaderLock();

        /**
         *  Unlock the Reader.
         */
        virtual void ReaderUnlock();

        /**
         *  Take the lock as a Writer.
         *  This allows only one thread access.
         */
        virtual void WriterLock();

        /**
         *  Unlock the Writer.
         */
        virtual void WriterUnlock();

    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this wrapper class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:
        /**
         *  Number of Writers waiting for the Resource Lock, including any
         *  current Writer already holdign it.
         */
        int WriteCount;

        /**
         *  Protect WriteCount.
         */
        SemaphoreHandle_t WriteLock;

        /**
         *  Lock to stop reader threads from starving a Writer.
         */
        SemaphoreHandle_t BlockReadersLock;
};


}
#endif
