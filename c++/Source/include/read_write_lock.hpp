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
#ifndef READ_WRITE_LOCK_HPP_
#define READ_WRITE_LOCK_HPP_


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
