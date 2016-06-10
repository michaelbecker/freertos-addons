/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* Standard includes. */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"


#define portMAX_INTERRUPTS				( ( uint32_t ) sizeof( uint32_t ) * 8UL ) /* The number of bits in an uint32_t. */
#define portNO_CRITICAL_NESTING 		( ( uint32_t ) 0 )


/*-----------------------------------------------------------*/

/* The WIN32 simulator runs each task in a thread.  The context switching is
managed by the threads, so the task stack does not have to be managed directly,
although the task stack is still used to hold an xThreadState structure this is
the only thing it will ever hold.  The structure indirectly maps the task handle
to a thread handle. */
typedef struct
{
	/* Handle of the thread that executes the task. */
	pthread_t pvThread;

} xThreadState;

/* Simulated interrupts waiting to be processed.  This is a bit mask where each
bit represents one interrupt, so a maximum of 32 interrupts can be simulated. */
static volatile uint32_t ulPendingInterrupts = 0UL;

/* An event used to inform the simulated interrupt processing thread (a high
priority thread that simulated interrupt processing) that an interrupt is
pending. */
pthread_cond_t InterruptEventCondVar = PTHREAD_COND_INITIALIZER;

/* Mutex used to protect all the simulated interrupt variables that are accessed
by multiple threads. */
pthread_mutex_t InterruptEventMutex = PTHREAD_MUTEX_INITIALIZER;

int InterruptEvent = 0;

/* The critical nesting count for the currently executing task.  This is
initialised to a non-zero value so interrupts do not become enabled during
the initialisation phase.  As each task has its own critical nesting value
ulCriticalNesting will get set to zero when the first task runs.  This
initialisation is probably not critical in this simulated environment as the
simulated interrupt handlers do not get created until the FreeRTOS scheduler is
started anyway. */
static uint32_t ulCriticalNesting = 9999UL;

/* Handlers for all the simulated software interrupts.  The first two positions
are used for the Yield and Tick interrupts so are handled slightly differently,
all the other interrupts can be user defined. */
static uint32_t (*ulIsrHandler[ portMAX_INTERRUPTS ])( void ) = { 0 };

/* Pointer to the TCB of the currently executing task. */
extern void *pxCurrentTCB;

/* Used to ensure nothing is processed during the startup sequence. */
static BaseType_t xPortRunning = pdFALSE;


/**
 *  Provides a count of the leading zeros in an integer.
 *
 *  @param mask we are counting zero bits in.
 *  @returns number of leading zeros
 */
static int
count_leading_zeros(unsigned int mask)
{
    int count = 0;

    if (mask == 0){
        return 32;
    }

    if (mask <= 0x0000FFFF) {
        count += 16;
        mask <<= 16;
    }

    if (mask <= 0x00FFFFFF) {
        count += 8;
        mask <<= 8;
    }

    if (mask <= 0x0FFFFFFF) {
        count += 4;
        mask <<= 4;
    }

    if (mask <= 0x3FFFFFFF) {
        count += 2;
        mask <<= 2;
    }

    if (mask <= 0x7FFFFFFF) {
        count += 1;
    }

    return count;
}

/**
 *  Provides a zero based index of the first MSb set 
 *  in the given mask.
 *
 *  @param index Pointer where we will return the zero based 
 *  index of the first MSb set.
 *  @param mask where we are searching.
 *  @returns zero if no bits are set. one of there is 
 *  some bit set.
 */
static int
first_leading_bit(  int *index,         /* [out] */
                    unsigned int mask)  /* [in]  */
{
    int num_leading_zeros = count_leading_zeros(mask);

    if (num_leading_zeros == 32){  
        return 0;
    }

    *index = 31 - num_leading_zeros;

    return 1;
}



/*-----------------------------------------------------------*/

/*
 * Created as a high priority thread, this function uses a timer to simulate
 * a tick interrupt being generated on an embedded target.  In this Windows
 * environment the timer does not achieve anything approaching real time
 * performance though.
 */
static unsigned long prvSimulatedPeripheralTimer( LPVOID lpParameter )
{
struct timespec delayPeriod;
struct timespec remainder;
int rc;

    /* Just to prevent compiler warnings. */
	( void ) lpParameter;

	for( ;; )
	{
		/* Wait until the timer expires and we can access the simulated interrupt
		variables.  *NOTE* this is not a 'real time' way of generating tick
		events as the next wake time should be relative to the previous wake
		time, not the time that Sleep() is called.  It is done this way to
		prevent overruns in this very non real time simulated/emulated
		environment. */
#define NS_IN_MS        (1000L * 1000L)
        delayPeriod.tv_sec = 0;
        delayPeriod.tv_nsec = portTICK_PERIOD_MS * NS_IN_MS;
        while (1)
        {
            rc = nanosleep(&delayPeriod, &remainder);
            if (rc == 0) 
            {
                break;
            }
            configASSERT(errno == EINTR);
            delayPeriod = remainder;
        }

		configASSERT( xPortRunning );

        pthread_mutex_lock( &InterruptEventMutex);

		/* The timer has expired, generate the simulated tick event. */
		ulPendingInterrupts |= ( 1 << portINTERRUPT_TICK );

		/* The interrupt is now pending - notify the simulated interrupt
		handler thread. */
		if( ulCriticalNesting == 0 )
		{
            InterruptEvent = 1;
            pthread_cond_signal( &InterruptEventCondVar );
		}

		/* Give back the mutex so the simulated interrupt handler unblocks
		and can	access the interrupt handler variables. */
        pthread_mutex_unlock( &InterruptEventMutex);
	}

	#ifdef __GNUC__
		/* Should never reach here - MingW complains if you leave this line out,
		MSVC complains if you put it in. */
		return 0;
	#endif
}
/*-----------------------------------------------------------*/

typedef void *(*pthread_start_routine_t) (void *)

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
int rc;
cpu_set_t cpuset;
xThreadState *pxThreadState = NULL;
int8_t *pcTopOfStack = ( int8_t * ) pxTopOfStack;

	/* In this simulated case a stack is not initialised, but instead a thread
	is created that will execute the task being created.  The thread handles
	the context switching itself.  The xThreadState object is placed onto
	the stack that was created for the task - so the stack buffer is still
	used, just not in the conventional way.  It will not be used for anything
	other than holding this structure. */
	pxThreadState = ( xThreadState * ) ( pcTopOfStack - sizeof( xThreadState ) );

	/* Create the thread itself. */
    // TODO - Create suspended?
    rc = pthread_create( &pxThreadState->pvThread, NULL, (pthread_start_routine_t)pxCode, pvParameters);
	configASSERT( rc == 0 );

    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    rc = pthread_setaffinity_np(pxThreadState->pvThread, sizeof(cpu_set_t), &cpuset);
	configASSERT( rc == 0 );

	SetThreadPriority( pxThreadState->pvThread, THREAD_PRIORITY_IDLE );

	return ( StackType_t * ) pxThreadState;
}

/*-----------------------------------------------------------*/
/*
 * Install an interrupt handler to be called by the simulated interrupt handler
 * thread.  The interrupt number must be above any used by the kernel itself
 * (at the time of writing the kernel was using interrupt numbers 0, 1, and 2
 * as defined above).  The number must also be lower than 32.
 *
 * Interrupt handler functions must return a non-zero value if executing the
 * handler resulted in a task switch being required.
 */
void vPortSetInterruptHandler( uint32_t ulInterruptNumber, uint32_t (*pvHandler)( void ) )
{
	if( ulInterruptNumber < portMAX_INTERRUPTS )
	{
        pthread_mutex_lock( &InterruptEventMutex);
		ulIsrHandler[ ulInterruptNumber ] = pvHandler;
        pthread_mutex_unlock( &InterruptEventMutex);
	}
}
/*-----------------------------------------------------------*/

/*
 * Process all the simulated interrupts - each represented by a bit in
 * ulPendingInterrupts variable.
 */
static void prvProcessSimulatedInterrupts( void )
{
uint32_t ulSwitchRequired, i;
xThreadState *pxThreadState;
void *pvObjectList[ 2 ];
CONTEXT xContext;

	/* Create a pending tick to ensure the first task is started as soon as
	this thread pends. */
	ulPendingInterrupts |= ( 1 << portINTERRUPT_TICK );

    pthread_mutex_lock( &InterruptEventMutex);
    InterruptEvent = 1;
    pthread_cond_signal( &InterruptEventCondVar );
    pthread_mutex_unlock( &InterruptEventMutex);

	xPortRunning = pdTRUE;
    
    pthread_mutex_lock( &InterruptEventMutex);

	for(;;)
	{
        while ( InterruptEvent != 1 )
            pthread_cond_wait( &InterruptEventCondVar, &InterruptEventMutex );

        InterruptEvent = 0;

		/* Used to indicate whether the simulated interrupt processing has
		necessitated a context switch to another task/thread. */
		ulSwitchRequired = pdFALSE;

		/* For each interrupt we are interested in processing, each of which is
		represented by a bit in the 32bit ulPendingInterrupts variable. */
		for( i = 0; i < portMAX_INTERRUPTS; i++ )
		{
			/* Is the simulated interrupt pending? */
			if( ulPendingInterrupts & ( 1UL << i ) )
			{
				/* Is a handler installed? */
				if( ulIsrHandler[ i ] != NULL )
				{
					/* Run the actual handler. */
					if( ulIsrHandler[ i ]() != pdFALSE )
					{
						ulSwitchRequired |= ( 1 << i );
					}
				}

				/* Clear the interrupt pending bit. */
				ulPendingInterrupts &= ~( 1UL << i );
			}
		}

		if( ulSwitchRequired != pdFALSE )
		{
			void *pvOldCurrentTCB;

			pvOldCurrentTCB = pxCurrentTCB;

			/* Select the next task to run. */
			vTaskSwitchContext();

			/* If the task selected to enter the running state is not the task
			that is already in the running state. */
			if( pvOldCurrentTCB != pxCurrentTCB )
			{
				/* Suspend the old thread. */
				pxThreadState = ( xThreadState *) *( ( size_t * ) pvOldCurrentTCB );
				SuspendThread( pxThreadState->pvThread );

				/* Ensure the thread is actually suspended by performing a 
				synchronous operation that can only complete when the thread is 
				actually suspended.  The below code asks for dummy register
				data. */
				xContext.ContextFlags = CONTEXT_INTEGER;
				( void ) GetThreadContext( pxThreadState->pvThread, &xContext );

				/* Obtain the state of the task now selected to enter the
				Running state. */
				pxThreadState = ( xThreadState * ) ( *( size_t *) pxCurrentTCB );
				ResumeThread( pxThreadState->pvThread );
			}
		}
	}
}

/*-----------------------------------------------------------*/
/*
 * Interrupt handlers used by the kernel itself.  These are executed from the
 * simulated interrupt handler thread.
 */
static uint32_t prvProcessYieldInterrupt( void )
{
	return pdTRUE;
}

/*-----------------------------------------------------------*/
static uint32_t prvProcessTickInterrupt( void )
{
uint32_t ulSwitchRequired;

	/* Process the tick itself. */
	configASSERT( xPortRunning );
	ulSwitchRequired = ( uint32_t ) xTaskIncrementTick();

	return ulSwitchRequired;
}

/*-----------------------------------------------------------*/
BaseType_t xPortStartScheduler( void )
{
int rc;
cpu_set_t cpuset;
pthread_t pvHandle;
int32_t lSuccess = pdPASS;
xThreadState *pxThreadState;

	/* Install the interrupt handlers used by the scheduler itself. */
	vPortSetInterruptHandler( portINTERRUPT_YIELD, prvProcessYieldInterrupt );
	vPortSetInterruptHandler( portINTERRUPT_TICK, prvProcessTickInterrupt );

	/* Set the priority of this thread such that it is above the priority of
	the threads that run tasks.  This higher priority is required to ensure
	simulated interrupts take priority over tasks. */
	pvHandle = pthread_self();

	if( lSuccess == pdPASS )
	{
		if( SetThreadPriority( pvHandle, THREAD_PRIORITY_NORMAL ) == 0 )
		{
			lSuccess = pdFAIL;
		}
	    CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        rc = pthread_setaffinity_np(pxThreadState->pvThread, sizeof(cpu_set_t), &cpuset);
	    configASSERT( rc == 0 );
    }

	if( lSuccess == pdPASS )
	{
		/* Start the thread that simulates the timer peripheral to generate
		tick interrupts.  The priority is set below that of the simulated
		interrupt handler so the interrupt event mutex is used for the
		handshake / overrun protection. */
        rc = pthread_create( &pvThread, NULL, (pthread_start_routine_t)prvSimulatedPeripheralTimer, NULL);
        configASSERT(rc == 0);

		SetThreadPriority( pvHandle, THREAD_PRIORITY_BELOW_NORMAL );
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        rc = pthread_setaffinity_np(pxThreadState->pvThread, sizeof(cpu_set_t), &cpuset);
        configASSERT( rc == 0 );
		ResumeThread( pvHandle );

		/* Start the highest priority task by obtaining its associated thread
		state structure, in which is stored the thread handle. */
		pxThreadState = ( xThreadState * ) *( ( size_t * ) pxCurrentTCB );
		ulCriticalNesting = portNO_CRITICAL_NESTING;

		/* Bump up the priority of the thread that is going to run, in the
		hope that this will assist in getting the Windows thread scheduler to
		behave as an embedded engineer might expect. */
		ResumeThread( pxThreadState->pvThread );

		/* Handle all simulated interrupts - including yield requests and
		simulated ticks. */
		prvProcessSimulatedInterrupts();
	}

	/* Would not expect to return from prvProcessSimulatedInterrupts(), so should
	not get here. */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortDeleteThread( void *pvTaskToDelete )
{
int rc;
xThreadState *pxThreadState;
uint32_t ulErrorCode;

	/* Remove compiler warnings if configASSERT() is not defined. */
	( void ) ulErrorCode;

	/* Find the handle of the thread being deleted. */
	pxThreadState = ( xThreadState * ) ( *( size_t *) pvTaskToDelete );

	/* Check that the thread is still valid, it might have been closed by
	vPortCloseRunningThread() - which will be the case if the task associated
	with the thread originally deleted itself rather than being deleted by a
	different task. */
	if( pxThreadState->pvThread != NULL )
	{
        pthread_mutex_lock( &InterruptEventMutex);

		rc = pthread_cancel(&pxThreadState->pvThread);
        configASSERT( rc == 0);
        
        rc = pthread_join(&pxThreadState->pvThread, NULL);
        configASSERT( rc == 0);

        pthread_mutex_unlock( &InterruptEventMutex);
	}
}
/*-----------------------------------------------------------*/

void vPortCloseRunningThread( void *pvTaskToDelete, volatile BaseType_t *pxPendYield )
{
xThreadState *pxThreadState;
void *pvThread;
uint32_t ulErrorCode;

	/* Remove compiler warnings if configASSERT() is not defined. */
	( void ) ulErrorCode;

	/* Find the handle of the thread being deleted. */
	pxThreadState = ( xThreadState * ) ( *( size_t *) pvTaskToDelete );
	pvThread = pxThreadState->pvThread;

	/* Raise the Windows priority of the thread to ensure the FreeRTOS scheduler
	does not run and swap it out before it is closed.  If that were to happen
	the thread would never run again and effectively be a thread handle and
	memory leak. */
	SetThreadPriority( pvThread, THREAD_PRIORITY_ABOVE_NORMAL );

	/* This function will not return, therefore a yield is set as pending to
	ensure a context switch occurs away from this thread on the next tick. */
	*pxPendYield = pdTRUE;

	/* Mark the thread associated with this task as invalid so
	vPortDeleteThread() does not try to terminate it. */
	pxThreadState->pvThread = NULL;

	/* Close the thread. */
	ulErrorCode = CloseHandle( pvThread );
	configASSERT( ulErrorCode );

    pthread_exit(NULL);
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* This function IS NOT TESTED! */
    exit(0);
}
/*-----------------------------------------------------------*/

void vPortGenerateSimulatedInterrupt( uint32_t ulInterruptNumber )
{
	configASSERT( xPortRunning );

	if( ulInterruptNumber < portMAX_INTERRUPTS )
	{
		/* Yield interrupts are processed even when critical nesting is non-zero. */
        pthread_mutex_lock( &InterruptEventMutex);
		ulPendingInterrupts |= ( 1 << ulInterruptNumber );

		/* The simulated interrupt is now held pending, but don't actually process it
		yet if this call is within a critical section.  It is possible for this to
		be in a critical section as calls to wait for mutexes are accumulative. */
		if( ulCriticalNesting == 0 )
		{
            InterruptEvent = 1;
			pthread_cond_signal( &InterruptEventCondVar );
		}

        pthread_mutex_unlock( &InterruptEventMutex);
	}
}

/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	if( xPortRunning == pdTRUE )
	{
		/* The interrupt event mutex is held for the entire critical section,
		effectively disabling (simulated) interrupts. */
        pthread_mutex_lock( &InterruptEventMutex);
		ulCriticalNesting++;
	}
	else
	{
		ulCriticalNesting++;
	}
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
int32_t lMutexNeedsReleasing;

	/* The interrupt event mutex should already be held by this thread as it was
	obtained on entry to the critical section. */

	lMutexNeedsReleasing = pdTRUE;

	if( ulCriticalNesting > portNO_CRITICAL_NESTING )
	{
		if( ulCriticalNesting == ( portNO_CRITICAL_NESTING + 1 ) )
		{
			ulCriticalNesting--;

			/* Were any interrupts set to pending while interrupts were
			(simulated) disabled? */
			if( ulPendingInterrupts != 0UL )
			{
				configASSERT( xPortRunning );
                InterruptEvent = 1;
			    pthread_cond_signal( &InterruptEventCondVar );

				/* Mutex will be released now, so does not require releasing
				on function exit. */
				lMutexNeedsReleasing = pdFALSE;
                pthread_mutex_unlock( &InterruptEventMutex);
			}
		}
		else
		{
			/* Tick interrupts will still not be processed as the critical
			nesting depth will not be zero. */
			ulCriticalNesting--;
		}
	}

	if( lMutexNeedsReleasing == pdTRUE )
	{
		configASSERT( xPortRunning );
        pthread_mutex_unlock( &InterruptEventMutex);
	}
}
/*-----------------------------------------------------------*/

