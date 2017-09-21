/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS C Add-ons project.
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
 *  The FreeRTOS C Add-ons project is free software: you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The FreeRTOS C Add-ons project is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the FreeRTOS C Add-ons project.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  NOTE: A modification to the GPL is included to allow you to distribute a
 *  combined work that includes FreeRTOS C Add-ons project
 *  without being obliged to provide the source
 *  code for proprietary components.
 * 
 *  The FreeRTOS C Add-ons project GPL exception text follows:
 * 
 *  Any FreeRTOS C Add-ons *source code*, whether modified or in it's 
 *  original release form, or whether in whole or in part, can only be 
 *  distributed by you under the terms of the GNU General Public License plus
 *  this exception.  An independent module is a module which is not derived
 *  from or based on FreeRTOS C Add-ons project.
 * 
 *  Clause 1:
 * 
 *  Linking FreeRTOS C Add-ons project  with other modules is making a 
 *  combined work based on FreeRTOS C Add-ons project. Thus, the terms 
 *  and conditions of the GNU General Public License V2 cover the
 *  whole combination.
 * 
 *  As a special exception, the copyright holders of FreeRTOS C Add-ons
 *  project give you permission to link FreeRTOS C Add-ons project with 
 *  independent modules to produce a statically linked executable, regardless
 *  of the license terms of these independent modules, and to copy and 
 *  distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the 
 *  terms and conditions of the license of that module.  An independent 
 *  module is a module which is not derived from or based on FreeRTOS C 
 *  Add-ons project.
 * 
 *  Clause 2:
 * 
 *  FreeRTOS C Add-ons project may not be used for any competitive or 
 *  comparative purpose, including the publication of any form of run time 
 *  or compile time metric, without the express permission of the copyright 
 *  holder(s) (this is the norm within the industry and is intended to ensure
 *  information accuracy).
 *  
 ***************************************************************************/
#include <stdlib.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "read_write_lock.h"


/*
 *  We need to differentiate which type of lock this is. We can 
 *  also use this as a sanity check, signature.
 */
enum ReaderWriterLockType {

    RwPreferReader = 0x72656164,    /* "read" */
    RwPreferWriter = 0x77726974     /* "writ" */
};


typedef struct RWLockPreferReader_t_ {

    /**
     *  Specify a type as well as using this as a signature.
     */
    enum ReaderWriterLockType Type;

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

} RWLockPreferReader_t;


typedef struct RWLockPreferWriter_t_ {

    /*
     *  --------------------------------------------------------
     *  THIS PORTION MUST MIRROR THE PREFER READER STRUCTURE
     */
    enum ReaderWriterLockType Type;

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

    /*
     *  --------------------------------------------------------
     *  WRITER ONLY ADDITION
     */

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

} RWLockPreferWriter_t;



ReadWriteLock_t *CreateReadWriteLockPreferReader(void)
{
    RWLockPreferReader_t *lock;

    lock = (RWLockPreferReader_t *)malloc(sizeof(RWLockPreferReader_t));
    if (!lock) {
        /*
         *  If the initial malloc fails, just get out.
         */
        return NULL;
    }

    lock->Type = RwPreferReader;
    
    lock->ReadCount = 0;

    /*
     *  Preload these with NULLs to allow simplified error handling.
     */
    lock->ReadLock = NULL;
    lock->ResourceLock = NULL;


    lock->ReadLock = xSemaphoreCreateMutex();
    if (lock->ReadLock == NULL) {
        goto ERROR;
    }

    /*
     *  ResourceLock CANNOT be a mutex. In FreeRTOS, as in most OS's,
     *  a thread is not allowed to unlock another thread's mutex. But
     *  the very nature of a Reader Lock allows an arbitrary ordering 
     *  of unlocks when multiple threads hold the reader lock. 
     *  Semaphores are not subject to this constraint.
     */
    lock->ResourceLock = xSemaphoreCreateBinary();
    if (lock->ResourceLock == NULL) {
        goto ERROR;
    }

    /*
     *  Initialize it as "full", so it behaves similar to a mutex.
     */
    xSemaphoreGive(lock->ResourceLock);

    return (ReadWriteLock_t *)lock;


    /*
     *  Handle most error cases here. This simplifies cleanup
     *  and is a common pattern in most OSs.
     */
ERROR:
    if (!lock->ReadLock) {
        vSemaphoreDelete(lock->ReadLock);
    } 

    if (!lock->ResourceLock) {
        vSemaphoreDelete(lock->ResourceLock);
    }

    free(lock);
    return NULL;
}


ReadWriteLock_t *CreateReadWriteLockPreferWriter(void)
{
    RWLockPreferWriter_t *lock;

    lock = (RWLockPreferWriter_t *)malloc(sizeof(RWLockPreferWriter_t));
    if (!lock) {
        /*
         *  If the initial malloc fails, just get out.
         */
        return NULL;
    }

    lock->Type = RwPreferWriter;
    
    lock->ReadCount = 0;
    lock->WriteCount = 0;

    /*
     *  Preload these with NULLs to allow simplified error handling.
     */
    lock->ReadLock = NULL;
    lock->WriteLock = NULL;
    lock->ResourceLock = NULL;
    lock->BlockReadersLock = NULL;

    lock->ReadLock = xSemaphoreCreateMutex();
    if (lock->ReadLock == NULL) {
        goto ERROR;
    }

    lock->WriteLock = xSemaphoreCreateMutex();
    if (lock->WriteLock == NULL) {
        goto ERROR;
    }

    /*
     *  ResourceLock CANNOT be a mutex. In FreeRTOS, as in most OS's,
     *  a thread is not allowed to unlock another thread's mutex. But
     *  the very nature of a Reader Lock allows an arbitrary ordering 
     *  of unlocks when multiple threads hold the reader lock. 
     *  Semaphores are not subject to this constraint.
     */
    lock->ResourceLock = xSemaphoreCreateBinary();
    if (lock->ResourceLock == NULL) {
        goto ERROR;
    }

    /*
     *  Initialize it as "full", so it behaves similar to a mutex.
     */
    xSemaphoreGive(lock->ResourceLock);

    /*
     *  BlockReadersLock CANNOT be a mutex. In FreeRTOS, as in most OS's,
     *  a thread is not allowed to unlock another thread's mutex. But
     *  the very nature of a Reader Lock allows an arbitrary ordering 
     *  of unlocks when multiple threads hold the reader lock. 
     *  Semaphores are not subject to this constraint.
     */
    lock->BlockReadersLock = xSemaphoreCreateBinary();
    if (lock->BlockReadersLock == NULL) {
        goto ERROR;
    }

    /*
     *  Initialize it as "full", so it behaves similar to a mutex.
     */
    xSemaphoreGive(lock->BlockReadersLock);

    return (ReadWriteLock_t *)lock;


    /*
     *  Handle most error cases here. This simplifies cleanup
     *  and is a common pattern in most OSs.
     */
ERROR:
    if (!lock->ReadLock) {
        vSemaphoreDelete(lock->ReadLock);
    } 

    if (!lock->WriteLock) {
        vSemaphoreDelete(lock->WriteLock);
    }

    if (!lock->ResourceLock) {
        vSemaphoreDelete(lock->ResourceLock);
    }

    if (!lock->BlockReadersLock) {
        vSemaphoreDelete(lock->BlockReadersLock);
    }

    free(lock);
    return NULL;
}


void FreeReadWriteLock(ReadWriteLock_t *lock)
{
    RWLockPreferReader_t *lockR = (RWLockPreferReader_t *)lock;
    RWLockPreferWriter_t *lockW = (RWLockPreferWriter_t *)lock;

    configASSERT(lockR != NULL);

    if (lockR->Type == RwPreferReader) {
        vSemaphoreDelete(lockR->ReadLock);
        vSemaphoreDelete(lockR->ResourceLock);
        free(lockR);
    }
    else if (lockW->Type == RwPreferWriter) {
        vSemaphoreDelete(lockW->ReadLock);
        vSemaphoreDelete(lockW->WriteLock);
        vSemaphoreDelete(lockW->ResourceLock);
        vSemaphoreDelete(lockW->BlockReadersLock);
        free(lockW);
    }
    else {
        configASSERT(!"Bad RW Lock type!");
    }
}


void ReaderLock(ReadWriteLock_t *lock)
{
    RWLockPreferReader_t *lockR = (RWLockPreferReader_t *)lock;
    RWLockPreferWriter_t *lockW = (RWLockPreferWriter_t *)lock;

    configASSERT(lockR != NULL);

    if (lockR->Type == RwPreferReader) {

        xSemaphoreTake(lockR->ReadLock, portMAX_DELAY);

        lockR->ReadCount++;
        if (lockR->ReadCount == 1) {
            xSemaphoreTake(lockR->ResourceLock, portMAX_DELAY);
        }

        xSemaphoreGive(lockR->ReadLock);
    }
    else if (lockW->Type == RwPreferWriter) {

        xSemaphoreTake(lockW->BlockReadersLock, portMAX_DELAY);
        xSemaphoreTake(lockW->ReadLock, portMAX_DELAY);

        lockW->ReadCount++;
        if (lockW->ReadCount == 1) {
            xSemaphoreTake(lockW->ResourceLock, portMAX_DELAY);
        }

        xSemaphoreGive(lockW->ReadLock);
        xSemaphoreGive(lockW->BlockReadersLock);
        
    }
    else {
        configASSERT(!"Bad RW Lock type!");
    }
}


void ReaderUnlock(ReadWriteLock_t *lock)
{
    RWLockPreferReader_t *lockR = (RWLockPreferReader_t *)lock;
    RWLockPreferWriter_t *lockW = (RWLockPreferWriter_t *)lock;

    configASSERT(lockR != NULL);

    if (lockR->Type == RwPreferReader) {

        xSemaphoreTake(lockR->ReadLock, portMAX_DELAY);

        lockR->ReadCount--;
        if (lockR->ReadCount == 0) {
            xSemaphoreGive(lockR->ResourceLock);
        }

        xSemaphoreGive(lockR->ReadLock);
    }
    else if (lockW->Type == RwPreferWriter) {

        xSemaphoreTake(lockW->ReadLock, portMAX_DELAY);

        lockW->ReadCount--;
        if (lockW->ReadCount == 0) {
            xSemaphoreGive(lockW->ResourceLock);
        }

        xSemaphoreGive(lockW->ReadLock);

    }
    else {
        configASSERT(!"Bad RW Lock type!");
    }
}


void WriterLock(ReadWriteLock_t *lock)
{
    RWLockPreferReader_t *lockR = (RWLockPreferReader_t *)lock;
    RWLockPreferWriter_t *lockW = (RWLockPreferWriter_t *)lock;

    configASSERT(lockR != NULL);

    if (lockR->Type == RwPreferReader) {
    
        xSemaphoreTake(lockR->ResourceLock, portMAX_DELAY);
        
    }
    else if (lockW->Type == RwPreferWriter) {

        xSemaphoreTake(lockW->WriteLock, portMAX_DELAY);

        lockW->WriteCount++;
        if (lockW->WriteCount == 1) {
            xSemaphoreTake(lockW->BlockReadersLock, portMAX_DELAY);
        }

        xSemaphoreGive(lockW->WriteLock);

        xSemaphoreTake(lockW->ResourceLock, portMAX_DELAY);

    }
    else {
        configASSERT(!"Bad RW Lock type!");
    }
}


void WriterUnlock(ReadWriteLock_t *lock)
{
    RWLockPreferReader_t *lockR = (RWLockPreferReader_t *)lock;
    RWLockPreferWriter_t *lockW = (RWLockPreferWriter_t *)lock;

    configASSERT(lockR != NULL);

    if (lockR->Type == RwPreferReader) {

        xSemaphoreGive(lockR->ResourceLock);
        
    }
    else if (lockW->Type == RwPreferWriter) {

        xSemaphoreGive(lockW->ResourceLock);

        xSemaphoreTake(lockW->WriteLock, portMAX_DELAY);

        lockW->WriteCount--;
        if (lockW->WriteCount == 0) {
            xSemaphoreGive(lockW->BlockReadersLock);
        }

        xSemaphoreGive(lockW->WriteLock);
            
    }
    else {
        configASSERT(!"Bad RW Lock type!");
    }
}



