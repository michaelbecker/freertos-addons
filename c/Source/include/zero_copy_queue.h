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
 ***************************************************************************/
#ifndef ZERO_COPY_QUEUE_H_
#define ZERO_COPY_QUEUE_H_


typedef void * ZeroCopyQueue_t;


ZeroCopyQueue_t ZcqCreateQueue( int itemSize,
                                int itemCount,
                                int Alignment);

void *ZcqAllocateItem(ZeroCopyQueue_t zcq);

void ZcqFreeItem(ZeroCopyQueue_t zcq, void *item);

void ZcqEnqueueItem(ZeroCopyQueue_t zcq, void *item, TickType_t Timeout);

void ZcqDequeueItem(ZeroCopyQueue_t zcq, void *item, TickType_t Timeout);


#endif

