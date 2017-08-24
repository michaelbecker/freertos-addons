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
 #include "event_groups.hpp"
 
 
using namespace cpp_freertos;


EventGroup::EventGroup()
{
	handle = xEventGroupCreate();
	
	if (handle == NULL) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
		throw EventGroupCreateException();
#else
		configASSERT(!"EventGroup Constructor Failed");
#endif
	}

}

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
EventGroup::EventGroup(StaticEventGroup_t *pxEventGroupBuffer)
{
	handle = xEventGroupCreateStatic(pxEventGroupBuffer);

	if (handle == NULL) {
#ifndef CPP_FREERTOS_NO_EXCEPTIONS
		throw EventGroupCreateException();
#else
		configASSERT(!"EventGroup Constructor Failed");
#endif
	}

}
#endif /* configSUPPORT_STATIC_ALLOCATION */

EventGroup::~EventGroup()
{
	vEventGroupDelete(handle);
}

EventBits_t EventGroup::Sync(	const EventBits_t uxBitsToSet, 
								const EventBits_t uxBitsToWaitFor, 
								TickType_t xTicksToWait)
{
	
	return xEventGroupSync(	handle, 
							uxBitsToSet, 
							uxBitsToWaitFor, 
							xTicksToWait);

}


EventBits_t EventGroup::WaitBits(	const EventBits_t uxBitsToWaitFor, 
									bool xClearOnExit, 
									bool xWaitForAllBits, 
									TickType_t xTicksToWait)
{
	
	return xEventGroupWaitBits(	handle, 
								uxBitsToWaitFor, 
								xClearOnExit ? pdTRUE : pdFALSE, 
								xWaitForAllBits ? pdTRUE : pdFALSE, 
								xTicksToWait);

}


EventBits_t EventGroup::ClearBits(const EventBits_t uxBitsToClear)
{

	return xEventGroupClearBits(handle, uxBitsToClear);

}


BaseType_t EventGroup::ClearBitsFromISR(const EventBits_t uxBitsToClear)
{
	
	return xEventGroupClearBitsFromISR(handle, uxBitsToClear);

}

EventBits_t EventGroup::GetBits()
{
	return xEventGroupGetBits(handle);
}

EventBits_t EventGroup::GetBitsFromISR()
{

	return xEventGroupGetBitsFromISR(handle);

}

EventBits_t EventGroup::SetBits(const EventBits_t uxBitsToSet)
{

	return xEventGroupSetBits(handle, uxBitsToSet);

}

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( INCLUDE_xTimerPendFunctionCall == 1 ) && ( configUSE_TIMERS == 1 ) )

BaseType_t EventGroup::SetBitsFromISR(const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken)
{

	return xEventGroupSetBitsFromISR(handle, uxBitsToSet, pxHigherPriorityTaskWoken);

}
#endif


 
