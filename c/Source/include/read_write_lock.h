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
#ifndef READ_WRITE_LOCK_H_
#define READ_WRITE_LOCK_H_


/**
 *  Handle for Reader/Writer locks.
 */
typedef void * ReadWriteLock_t;


/**
 *  Create a Reader/Writer lock that offers preference 
 *  to all readers.
 *
 *  @return A handle to the lock, or NULL on failure.
 */
ReadWriteLock_t *CreateReadWriteLockPreferReader(void);


/**
 *  Create a Reader/Writer lock that offers preference 
 *  to all writers.
 *
 *  @return A handle to the lock, or NULL on failure.
 */
ReadWriteLock_t *CreateReadWriteLockPreferWriter(void);


/**
 *  Free an already allocated Reader/Writer lock.
 *
 *  @param lock The lock you are freeing.
 */
void FreeReadWriteLock(ReadWriteLock_t *lock);


/**
 *  Lock as a Reader.
 *
 *  This allows you shared read only access to whatever is being 
 *  protected by this lock.
 *
 *  @param Lock The lock you are taking.
 */
void ReaderLock(ReadWriteLock_t *Lock);

/**
 *  Unlock as a Reader. 
 *
 *  You should have already locked this using ReaderLock().
 *
 *  @param Lock The lock you are releasing.
 */
void ReaderUnlock(ReadWriteLock_t *Lock);

/**
 *  Lock as a Writer.
 *
 *  This allows you exclusive read / write access to whatever is being
 *  protected by this lock.
 *
 *  @param Lock The lock you are taking.
 */
void WriterLock(ReadWriteLock_t *Lock);

/**
 *  Unlock as a Writer. 
 *
 *  You should have already locked this using WriterLock().
 *
 *  @param Lock The lock you are releasing.
 */
void WriterUnlock(ReadWriteLock_t *Lock);


#endif

