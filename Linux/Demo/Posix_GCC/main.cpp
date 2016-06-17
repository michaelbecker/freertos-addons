#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

int main (void)
{
    return 0;
}


void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
    printf("ASSERT: %s : %d\n", pcFileName, (int)ulLine);
    while (1);
}


unsigned long ulGetRunTimeCounterValue(void)
{
    return 0;
}

void vConfigureTimerForRunTimeStats(void)
{
    return;
}

extern "C" void vApplicationTickHook(void);
void vApplicationTickHook(void)
{
    return;
}


extern "C" void vApplicationIdleHook(void);
void vApplicationIdleHook(void)
{
}


extern "C" void vApplicationMallocFailedHook(void);
void vApplicationMallocFailedHook(void)
{
	while(1);
}

