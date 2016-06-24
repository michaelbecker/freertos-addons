/*
 *  Copyright (C) 2016 Michael Becker (michael.f.becker@gmail.com
 *  Modified from code from William Davy (william.davy@wittenstein.co.uk)
 *
 *  Modifications (not exhaustive):
 *  ---------------------------------------------
 *  + Updated to work with FreeRTOS v8.2.3
 *
 *  + Updated code that works with pthreads to 
 *    treat pthread_t data as opaque data structures,
 *    as specified in the POSIX standard. Likewise, 
 *    cannot initialize them to NULL, not portable.
 *
 *  + Since FreeRTOS is not multicore, we are forcing 
 *    all of the pthreads that simulate FreeRTOS tasks 
 *    to execute on a single core.
 *
 */
/*
    Copyright (C) 2009 William Davy - william.davy@wittenstein.co.uk
    Contributed to FreeRTOS.org V5.3.0.

    This file is part of the FreeRTOS.org distribution.

    FreeRTOS.org is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License (version 2) as published
    by the Free Software Foundation and modified by the FreeRTOS exception.

    FreeRTOS.org is distributed in the hope that it will be useful,    but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details.

    You should have received a copy of the GNU General Public License along
    with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
    Temple Place, Suite 330, Boston, MA  02111-1307  USA.

    A special exception to the GPL is included to allow you to distribute a
    combined work that includes FreeRTOS.org without being obliged to provide
    the source code for any proprietary components.  See the licensing section
    of http://www.FreeRTOS.org for full details.


    ***************************************************************************
    *                                                                         *
    * Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
    *                                                                         *
    * This is a concise, step by step, 'hands on' guide that describes both   *
    * general multitasking concepts and FreeRTOS specifics. It presents and   *
    * explains numerous examples that are written using the FreeRTOS API.     *
    * Full source code for all the examples is provided in an accompanying    *
    * .zip file.                                                              *
    *                                                                         *
    ***************************************************************************

    1 tab == 4 spaces!

    Please ensure to read the configuration and relevant port sections of the
    online documentation.

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the Posix port.
 *----------------------------------------------------------*/

#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"


/**
 *  Provides a count of the leading zeros in an integer.
 *
 *  @param mask we are counting zero bits in.
 *  @returns number of leading zeros
 */
int
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
int
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


/* Each task maintains its own interrupt status in the critical nesting variable. */
typedef struct ThreadState_t_
{
    pthread_t       Thread;
    int             Valid;    /* Treated as a boolean */
    xTaskHandle     hTask;
    portBASE_TYPE   uxCriticalNesting;
    pdTASK_CODE     pxCode;
    void            *pvParams;
    int             Nice;

} ThreadState_t;


#define MAX_NUMBER_OF_TASKS ( _POSIX_THREAD_THREADS_MAX )
static ThreadState_t pxThreads[MAX_NUMBER_OF_TASKS];

static pthread_once_t hSigSetupThread = PTHREAD_ONCE_INIT;

static pthread_mutex_t xSuspendResumeThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t xSingleThreadMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t hMainThread;

static volatile portBASE_TYPE xSentinel = 0;
static volatile portBASE_TYPE xSchedulerEnd = pdFALSE;
static volatile portBASE_TYPE xInterruptsEnabled = pdTRUE;
static volatile portBASE_TYPE xServicingTick = pdFALSE;
static volatile portBASE_TYPE xPendYield = pdFALSE;
static volatile portLONG lIndexOfLastAddedTask = 0;
static volatile portBASE_TYPE uxCriticalNesting;


/**
 *  pthread cleanup routine, always executed on pthread exit.
 */
static void DeleteThreadCleanupRoutine( void *Parameter )
{
    ThreadState_t *State = (ThreadState_t *)Parameter;

    State->Valid = 0;
    State->hTask = (xTaskHandle)NULL;
    if ( State->uxCriticalNesting > 0 )
    {
        State->uxCriticalNesting = 0;
        uxCriticalNesting = 0;
        vPortEnableInterrupts();
    }
}


/**
 *  Sends SIG_SUSPEND to target thread.
 */
static void SuspendThread( pthread_t Thread )
{
    pthread_mutex_lock( &xSuspendResumeThreadMutex );

    /* Set-up for the Suspend Signal handler? */
    xSentinel = 0;
    pthread_mutex_unlock( &xSuspendResumeThreadMutex );

    pthread_kill( Thread, SIG_SUSPEND );
    while ( ( xSentinel == 0 ) && ( pdTRUE != xServicingTick ) )
    {
        sched_yield();
    }
}


/**
 *  Signal handler for SIG_SUSPEND
 */
static void SuspendSignalHandler(int sig)
{
    sigset_t xSignals;
    int rc;

    /* Only interested in the resume signal. */
    sigemptyset( &xSignals );
    sigaddset( &xSignals, SIG_RESUME );
    xSentinel = 1;

    /* Unlock the Single thread mutex to allow the resumed task to continue. */
    pthread_mutex_unlock(&xSingleThreadMutex);

    /* Wait on the resume signal. */
    rc = sigwait(&xSignals, &sig);
    assert(rc == 0);

    /* Will resume here when the SIG_RESUME signal is received. */
    /* Need to set the interrupts based on the task's critical nesting. */
    if ( uxCriticalNesting == 0 )
    {
        vPortEnableInterrupts();
    }
    else
    {
        vPortDisableInterrupts();
    }
}


/**
 *  Signal handler for SIG_RESUME.
 */
static void ResumeSignalHandler(int sig)
{
    (void)sig;

    /* Yield the Scheduler to ensure that the yielding thread completes. */
    pthread_mutex_lock(&xSingleThreadMutex);

    pthread_mutex_unlock(&xSingleThreadMutex);
}


/**
 *  Sends SIG_RESUME to target thread.
 */
static void ResumeThread( pthread_t Thread )
{
    pthread_mutex_lock(&xSuspendResumeThreadMutex);

    if ( !pthread_equal(pthread_self(), Thread))
    {
        pthread_kill( Thread, SIG_RESUME );
    }

    pthread_mutex_unlock( &xSuspendResumeThreadMutex );
}


/**
 *  Utility function to lookup a pthread_t based on 
 *  a FreeRTOS Task Handle.
 */
static void LookupThread(xTaskHandle hTask, pthread_t *Thread)
{
    int i;
    
    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if (pxThreads[i].hTask == hTask)
        {
            *Thread = pxThreads[i].Thread;
            return;
        }
    }

    assert(!"Failed finding pthread for task mapping!");
}


static void prvSetTaskCriticalNesting( pthread_t Thread, portBASE_TYPE uxNesting )
{
    int i;

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if ( pthread_equal(pxThreads[i].Thread, Thread) )
        {
            pxThreads[i].uxCriticalNesting = uxNesting;
            return;
        }
    }

    assert(!"Failed finding pthread for task mapping!");
}


static portBASE_TYPE prvGetTaskCriticalNesting( pthread_t Thread )
{
    unsigned portBASE_TYPE uxNesting = 0;
    int i;

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if ( pthread_equal(pxThreads[i].Thread, Thread) )
        {
            uxNesting = pxThreads[i].uxCriticalNesting;
            return uxNesting;
        }
    }
    
    assert(!"Failed finding pthread for task mapping!");
}


/**
 *  Signal handler for SIG_TICK.
 */
static void TickSignalHandler( int sig )
{
    pthread_t ThreadToSuspend;
    pthread_t ThreadToResume;

    (void)sig;

    if ((pdTRUE == xInterruptsEnabled) && (pdTRUE != xServicingTick))
    {
        if ( 0 == pthread_mutex_trylock( &xSingleThreadMutex ) )
        {
            xServicingTick = pdTRUE;

            LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToSuspend);

            /* Select Next Task. */
#if ( configUSE_PREEMPTION == 1 )
            vTaskSwitchContext();
#endif
            LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToResume);

            /* The only thread that can process this tick is the running thread. */
            if ( !pthread_equal(ThreadToSuspend, ThreadToResume) )
            {
                /* Remember and switch the critical nesting. */
                prvSetTaskCriticalNesting( ThreadToSuspend, uxCriticalNesting );
                uxCriticalNesting = prvGetTaskCriticalNesting( ThreadToResume );
                /* Resume next task. */
                ResumeThread( ThreadToResume );
                /* Suspend the current task. */
                SuspendThread( ThreadToSuspend );
            }
            else
            {
                /* Release the lock as we are Resuming. */
                pthread_mutex_unlock( &xSingleThreadMutex );
            }
            xServicingTick = pdFALSE;
        }
        else
        {
            xPendYield = pdTRUE;
        }
    }
    else
    {
        xPendYield = pdTRUE;
    }
}


/**
 *  Only run once!
 */
static void prvSetupSignalsAndSchedulerPolicy( void )
{
    /* The following code would allow for configuring the scheduling of this task as a Real-time task.
     * The process would then need to be run with higher privileges for it to take affect.
    int iPolicy;
    int iResult;
    int iSchedulerPriority;
    iResult = pthread_getschedparam( pthread_self(), &iPolicy, &iSchedulerPriority );
    iResult = pthread_attr_setschedpolicy( &xThreadAttributes, SCHED_FIFO );
    iPolicy = SCHED_FIFO;
    iResult = pthread_setschedparam( pthread_self(), iPolicy, &iSchedulerPriority );        */

    struct sigaction sigsuspendself, sigresume, sigtick;

    memset(pxThreads, 0, sizeof(pxThreads));

    sigsuspendself.sa_flags = 0;
    sigsuspendself.sa_handler = SuspendSignalHandler;
    sigfillset( &sigsuspendself.sa_mask );

    sigresume.sa_flags = 0;
    sigresume.sa_handler = ResumeSignalHandler;
    sigfillset( &sigresume.sa_mask );

    sigtick.sa_flags = 0;
    sigtick.sa_handler = TickSignalHandler;
    sigfillset( &sigtick.sa_mask );

    if ( 0 != sigaction( SIG_SUSPEND, &sigsuspendself, NULL ) )
    {
        printf( "Problem installing SIG_SUSPEND_SELF\n" );
    }
    if ( 0 != sigaction( SIG_RESUME, &sigresume, NULL ) )
    {
        printf( "Problem installing SIG_RESUME\n" );
    }
    if ( 0 != sigaction( SIG_TICK, &sigtick, NULL ) )
    {
        printf( "Problem installing SIG_TICK\n" );
    }
    printf( "Running as PID: %d\n", getpid() );

    /*
     *  Also save the first thread as the main thread.
     */
    hMainThread = pthread_self();
}


/**
 *  Actual pthreads code, wrapper around FreeRTOS task.
 */
static void *ThreadStartWrapper( void * pvParams )
{
    ThreadState_t *State = (ThreadState_t *)pvParams;

    if (State->Nice != 0)
        nice(State->Nice);
    
    pthread_cleanup_push( DeleteThreadCleanupRoutine, State );

    pthread_mutex_lock(&xSingleThreadMutex); 
    SuspendThread( pthread_self() );

    State->pxCode( State->pvParams );

    /* make sure we execute DeleteThreadCleanupRoutine */
    pthread_cleanup_pop( 1 );

    return (void *)NULL;
}


/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
    int rc;
    int i;
    pthread_attr_t xThreadAttributes;
    cpu_set_t cpuset;
 
    pthread_once( &hSigSetupThread, prvSetupSignalsAndSchedulerPolicy );

    /* No need to join the threads. */
    pthread_attr_init( &xThreadAttributes );
    pthread_attr_setdetachstate( &xThreadAttributes, PTHREAD_CREATE_DETACHED );

    vPortEnterCritical();

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++ )
    {
        if (pxThreads[i].Valid == 0)
        {
            lIndexOfLastAddedTask = i;
            break;
        }
    }

    /* No more free threads, please increase the maximum. */
    assert(i < MAX_NUMBER_OF_TASKS);


    /* Add the task parameters. */
    pxThreads[lIndexOfLastAddedTask].pxCode = pxCode;
    pxThreads[lIndexOfLastAddedTask].pvParams = pvParameters;

    /* Create the new pThread. */
    pthread_mutex_lock(&xSingleThreadMutex);

    xSentinel = 0;

    rc = pthread_create(&pxThreads[lIndexOfLastAddedTask].Thread, 
                        &xThreadAttributes, 
                        ThreadStartWrapper, 
                        (void *)&pxThreads[lIndexOfLastAddedTask]
                        );
    if (rc != 0)
    {
        /* Thread create failed, signal the failure */
        pxTopOfStack = 0;
    }
    else 
    {
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);

        rc = pthread_setaffinity_np(pxThreads[lIndexOfLastAddedTask].Thread, 
                                    sizeof(cpu_set_t), 
                                    &cpuset);
	    configASSERT( rc == 0 );
        pxThreads[lIndexOfLastAddedTask].Valid = 1;
    }

    /* Wait until the task suspends. */
    pthread_mutex_unlock( &xSingleThreadMutex );

    while ( xSentinel == 0 );
    vPortExitCritical();

    return pxTopOfStack;
}


/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
static void prvSetupTimerInterrupt( void )
{
    int rc;
    struct itimerval itimer, oitimer;
    portTickType xMicroSeconds = portTICK_RATE_MICROSECONDS;

    /* Initialise the structure with the current timer information. */
    rc = getitimer( TIMER_TYPE, &itimer);
    assert(rc == 0);

    /* Set the interval between timer events. */
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = xMicroSeconds;

    /* Set the current count-down. */
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = xMicroSeconds;

    /* Set-up the timer interrupt. */
    rc = setitimer( TIMER_TYPE, &itimer, &oitimer );
    assert(rc == 0);
}


/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
    int iSignal;
    sigset_t xSignals;
    sigset_t xSignalToBlock;
    sigset_t xSignalsBlocked;
    pthread_t FirstThread;

    /* Establish the signals to block before they are needed. */
    sigfillset( &xSignalToBlock );

    /* Block until the end */
    pthread_sigmask( SIG_SETMASK, &xSignalToBlock, &xSignalsBlocked );

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    prvSetupTimerInterrupt();

    /* Start the first task. Will not return unless all threads are killed. */
    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    vPortEnableInterrupts();

    LookupThread( xTaskGetCurrentTaskHandle(), &FirstThread);

    /* Start the first task. */
    ResumeThread(FirstThread);


    /* This is the end signal we are looking for. */
    sigemptyset( &xSignals );
    sigaddset( &xSignals, SIG_RESUME );

    while ( pdTRUE != xSchedulerEnd )
    {
        if ( 0 != sigwait( &xSignals, &iSignal ) )
        {
            printf( "Main thread spurious signal: %d\n", iSignal );
        }
    }

    printf( "Cleaning Up, Exiting.\n" );
    /* Cleanup the mutexes */
    pthread_mutex_destroy( &xSuspendResumeThreadMutex );
    pthread_mutex_destroy( &xSingleThreadMutex );

    /* Should not get here! */
    return 0;
}


void vPortEndScheduler( void )
{
    int i;

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if ( pxThreads[i].Valid )
        {
            /* Kill all of the threads, they are in the detached state. */
            pthread_cancel(pxThreads[i].Thread );
            pxThreads[i].Valid = 0;
        }
    }

    /* Signal the scheduler to exit its loop. */
    xSchedulerEnd = pdTRUE;

    pthread_kill( hMainThread, SIG_RESUME );
}


void vPortYieldFromISR( void )
{
    /* Calling Yield from a Interrupt/Signal handler often doesn't work because the
     * xSingleThreadMutex is already owned by an original call to Yield. Therefore,
     * simply indicate that a yield is required soon.
     */
    xPendYield = pdTRUE;
}


void vPortEnterCritical( void )
{
    vPortDisableInterrupts();
    uxCriticalNesting++;
}


void vPortExitCritical( void )
{
    /* Check for unmatched exits. */
    if ( uxCriticalNesting > 0 )
    {
        uxCriticalNesting--;
    }

    /* If we have reached 0 then re-enable the interrupts. */
    if( uxCriticalNesting == 0 )
    {
        /* Have we missed ticks? This is the equivalent of pending an interrupt. */
        if ( pdTRUE == xPendYield )
        {
            xPendYield = pdFALSE;
            vPortYield();
        }
        vPortEnableInterrupts();
    }
}


void vPortYield( void )
{
    pthread_t ThreadToSuspend;
    pthread_t ThreadToResume;

    pthread_mutex_lock(&xSingleThreadMutex);

    LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToSuspend);

    vTaskSwitchContext();

    LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToResume);

    if ( !pthread_equal(ThreadToSuspend, ThreadToResume) )
    {
        /* Remember and switch the critical nesting. */
        prvSetTaskCriticalNesting( ThreadToSuspend, uxCriticalNesting );
        uxCriticalNesting = prvGetTaskCriticalNesting( ThreadToResume );
        /* Switch tasks. */
        ResumeThread( ThreadToResume );
        SuspendThread( ThreadToSuspend );
    }
    else
    {
        /* Yielding to self */
        pthread_mutex_unlock( &xSingleThreadMutex );
    }
}


void vPortDisableInterrupts( void )
{
    xInterruptsEnabled = pdFALSE;
}


void vPortEnableInterrupts( void )
{
    xInterruptsEnabled = pdTRUE;
}


portBASE_TYPE xPortSetInterruptMask( void )
{
    portBASE_TYPE xReturn = xInterruptsEnabled;
    xInterruptsEnabled = pdFALSE;
    return xReturn;
}


void vPortClearInterruptMask( portBASE_TYPE xMask )
{
    xInterruptsEnabled = xMask;
}


/**
 *  Public API used in tasks.c
 */
void vPortForciblyEndThread( void *pxTaskToDelete )
{
    xTaskHandle hTaskToDelete = ( xTaskHandle )pxTaskToDelete;
    pthread_t ThreadToDelete;
    pthread_t ThreadToResume;

    pthread_mutex_lock(&xSingleThreadMutex);

    LookupThread(hTaskToDelete, &ThreadToDelete);
    LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToResume);

    if ( pthread_equal(ThreadToResume, ThreadToDelete) )
    {
        /* This is a suicidal thread, need to select a different task to run. */
        vTaskSwitchContext();
        LookupThread(xTaskGetCurrentTaskHandle(), &ThreadToResume);
    }

    if ( !pthread_equal(pthread_self(), ThreadToDelete) )
    {
        /* Cancelling a thread that is not me. */
 
        /* Send a signal to wake the task so that it definitely cancels. */
        pthread_testcancel();
        pthread_cancel( ThreadToDelete );
 
        /* Pthread Clean-up function will note the cancellation. */
        pthread_mutex_unlock( &xSingleThreadMutex );
    }
    else
    {
        /* Resume the other thread. */
        ResumeThread( ThreadToResume );
        /* Pthread Clean-up function will note the cancellation. */
        /* Release the execution. */
        uxCriticalNesting = 0;
        vPortEnableInterrupts();
        pthread_mutex_unlock( &xSingleThreadMutex );
        /* Commit suicide */
        pthread_exit( (void *)1 );
    }
}


/**
 *  Used for traceTASK_CREATE in FreeRTOS.
 */
void vPortAddTaskHandle( void *pxTaskHandle )
{
    int i;

    pxThreads[lIndexOfLastAddedTask].hTask = (xTaskHandle)pxTaskHandle;

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++)
    {
        if ( pthread_equal(pxThreads[i].Thread, pxThreads[lIndexOfLastAddedTask].Thread))
        {
            if ( pxThreads[i].hTask != pxThreads[ lIndexOfLastAddedTask ].hTask )
            {
                pxThreads[i].Valid = 0;
                pxThreads[i].hTask = NULL;
                pxThreads[i].uxCriticalNesting = 0;
            }
        }
    }
}


void vPortFindTicksPerSecond( void )
{
    /* Needs to be reasonably high for accuracy. */
    unsigned long ulTicksPerSecond = sysconf(_SC_CLK_TCK);
    printf( "Timer Resolution for Run TimeStats is %ld ticks per second.\n", ulTicksPerSecond );
}


unsigned long ulPortGetTimerValue( void )
{
    struct tms xTimes;

    (void)times( &xTimes );
    /* Return the application code times.
     * The timer only increases when the application code is actually running
     * which means that the total execution times should add up to 100%.
     */
    return ( unsigned long ) xTimes.tms_utime;
}



