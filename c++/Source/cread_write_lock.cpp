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
#include "read_write_lock.hpp"


using namespace cpp_freertos;


ReadWriteLock::ReadWriteLock()
    : ReadCount(0)
{
    ReadLock = xSemaphoreCreateMutex();
    if (ReadLock == NULL) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw ReadWriteLockCreateException();
#else
        configASSERT(!"ReadWriteLock Constructor Failed");
#endif
    }

    //
    //  ResourceLock CANNOT be a mutex. In FreeRTOS, as in most OS's,
    //  a thread is not allowed to unlock another thread's mutex. But
    //  the very nature of a Reader Lock allows an arbitrary ordering 
    //  of unlocks when multiple threads hold the reader lock. 
    //  Semaphores are not subject to this constraint.
    //
    ResourceLock = xSemaphoreCreateBinary();
    if (ResourceLock == NULL) {
        vSemaphoreDelete(ReadLock);
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw ReadWriteLockCreateException();
#else
        configASSERT(!"ReadWriteLock Constructor Failed");
#endif
    }

    //
    //  Initialize it as "full", so it behaves similar to a mutex.
    //
    xSemaphoreGive(ResourceLock);
}


ReadWriteLock::~ReadWriteLock()
{
    vSemaphoreDelete(ReadLock);
    vSemaphoreDelete(ResourceLock);
}


void ReadWriteLockPreferReader::ReaderLock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount++;
    if (ReadCount == 1) {
        xSemaphoreTake(ResourceLock, portMAX_DELAY);
    }

    xSemaphoreGive(ReadLock);
}


void ReadWriteLockPreferReader::ReaderUnlock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount--;
    if (ReadCount == 0) {
        xSemaphoreGive(ResourceLock);
    }

    xSemaphoreGive(ReadLock);
}


void ReadWriteLockPreferReader::WriterLock()
{
    xSemaphoreTake(ResourceLock, portMAX_DELAY);
}


void ReadWriteLockPreferReader::WriterUnlock()
{
    xSemaphoreGive(ResourceLock);
}


ReadWriteLockPreferWriter::ReadWriteLockPreferWriter()
    : ReadWriteLock(),
      WriteCount(0)
{
    WriteLock = xSemaphoreCreateMutex();
    if (WriteLock == NULL) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw ReadWriteLockCreateException();
#else
        configASSERT(!"ReadWriteLockPreferWriter Constructor Failed");
#endif
    }

    //
    //  BlockReadersLock CANNOT be a mutex. In FreeRTOS, as in most OS's,
    //  a thread is not allowed to unlock another thread's mutex. But
    //  the very nature of a Reader Lock allows an arbitrary ordering 
    //  of unlocks when multiple threads hold the reader lock. 
    //  Semaphores are not subject to this constraint.
    //
    BlockReadersLock = xSemaphoreCreateBinary();
    if (BlockReadersLock == NULL) {
        vSemaphoreDelete(WriteLock);
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
        throw ReadWriteLockCreateException();
#else
        configASSERT(!"ReadWriteLockPreferWriter Constructor Failed");
#endif
    }

    //
    //  Initialize it as "full", so it behaves similar to a mutex.
    //
    xSemaphoreGive(BlockReadersLock);
}


ReadWriteLockPreferWriter::~ReadWriteLockPreferWriter()
{
    vSemaphoreDelete(WriteLock);
    vSemaphoreDelete(BlockReadersLock);
}


void ReadWriteLockPreferWriter::ReaderLock()
{
    xSemaphoreTake(BlockReadersLock, portMAX_DELAY);
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount++;
    if (ReadCount == 1) {
        xSemaphoreTake(ResourceLock, portMAX_DELAY);
    }

    xSemaphoreGive(ReadLock);
    xSemaphoreGive(BlockReadersLock);
}


void ReadWriteLockPreferWriter::ReaderUnlock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount--;
    if (ReadCount == 0) {
        xSemaphoreGive(ResourceLock);
    }

    xSemaphoreGive(ReadLock);
}


void ReadWriteLockPreferWriter::WriterLock()
{
    xSemaphoreTake(WriteLock, portMAX_DELAY);

    WriteCount++;
    if (WriteCount == 1) {
        xSemaphoreTake(BlockReadersLock, portMAX_DELAY);
    }

    xSemaphoreGive(WriteLock);

    xSemaphoreTake(ResourceLock, portMAX_DELAY);
}


void ReadWriteLockPreferWriter::WriterUnlock()
{
    xSemaphoreGive(ResourceLock);

    xSemaphoreTake(WriteLock, portMAX_DELAY);

    WriteCount--;
    if (WriteCount == 0) {
        xSemaphoreGive(BlockReadersLock);
    }

    xSemaphoreGive(WriteLock);
}
