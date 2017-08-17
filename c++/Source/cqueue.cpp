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
#include "queue.hpp"
#include "task.h"
#include "queue.h"

using namespace cpp_freertos;

Queue::Queue(UBaseType_t maxItems, UBaseType_t itemSize) {
	handle = xQueueCreate(maxItems, itemSize);

	if (handle == NULL) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
		throw QueueCreateException();
#else
		configASSERT(!"Queue Constructor Failed");
#endif
	}
}

Queue::~Queue() {
	vQueueDelete(handle);
}

bool Queue::Enqueue(void *item) {
	BaseType_t success;

	success = xQueueSendToBack(handle, item, portMAX_DELAY);

	return success == pdTRUE ? true : false;
}

bool Queue::Enqueue(void *item, TickType_t Timeout) {
	BaseType_t success;

	success = xQueueSendToBack(handle, item, Timeout);

	return success == pdTRUE ? true : false;
}

bool Queue::Dequeue(void *item, TickType_t Timeout) {
	BaseType_t success;

	success = xQueueReceive(handle, item, Timeout);

	return success == pdTRUE ? true : false;
}

bool Queue::Peek(void *item, TickType_t Timeout) {
	BaseType_t success;

	success = xQueuePeek(handle, item, Timeout);

	return success == pdTRUE ? true : false;
}

bool Queue::EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken) {
	BaseType_t success;

	success = xQueueSendToBackFromISR(handle, item, pxHigherPriorityTaskWoken);

	return success == pdTRUE ? true : false;
}

bool Queue::DequeueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken) {
	BaseType_t success;

	success = xQueueReceiveFromISR(handle, item, pxHigherPriorityTaskWoken);

	return success == pdTRUE ? true : false;
}

bool Queue::PeekFromISR(void *item) {
	BaseType_t success;

	success = xQueuePeekFromISR(handle, item);

	return success == pdTRUE ? true : false;
}

bool Queue::IsEmpty() {
	UBaseType_t cnt = uxQueueMessagesWaiting(handle);

	return cnt == 0 ? true : false;
}

bool Queue::IsFull() {
	UBaseType_t cnt = uxQueueSpacesAvailable(handle);

	return cnt == 0 ? true : false;
}

void Queue::Flush() {
	xQueueReset(handle);
}

UBaseType_t Queue::NumItems() const {
	return uxQueueMessagesWaiting(handle);
}

UBaseType_t Queue::NumItemsFromISR() const {
	return uxQueueMessagesWaitingFromISR(handle);
}

UBaseType_t Queue::NumSpacesLeft() const {
	return uxQueueSpacesAvailable(handle);
}

UBaseType_t Queue::NumSpacesLeftFromISR() const {
	return uxQueueSpacesAvailableFromISR(this->handle);
}

Deque::Deque(UBaseType_t maxItems, UBaseType_t itemSize) :
		Queue(maxItems, itemSize) {
}

bool Deque::EnqueueToFront(void *item, TickType_t Timeout) {
	BaseType_t success;

	success = xQueueSendToFront(handle, item, Timeout);

	return success == pdTRUE ? true : false;
}

bool Deque::EnqueueToFrontFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken) {
	BaseType_t success;

	success = xQueueSendToFrontFromISR(handle, item, pxHigherPriorityTaskWoken);

	return success == pdTRUE ? true : false;
}

BinaryQueue::BinaryQueue(UBaseType_t itemSize) :
		Queue(1, itemSize) {
}

bool BinaryQueue::Enqueue(void *item) {
	(void) xQueueOverwrite(handle, item);
	return true;
}

bool BinaryQueue::EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken) {
	(void) xQueueOverwriteFromISR(handle, item, pxHigherPriorityTaskWoken);
	return true;
}
