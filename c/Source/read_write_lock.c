/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS Add-ons project.
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
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so,subject to the
 *  following conditions:
 *
 *  + The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *  + Credit is appreciated, but not required, if you find this project
 *    useful enough to include in your application, product, device, etc.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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



