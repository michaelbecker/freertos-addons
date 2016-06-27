#include "ticks.hpp"


using namespace std;
using namespace cpp_freertos;


#if ( configUSE_TICK_HOOK == 1 )

list<ApplicationTickHookFcn *> Ticks::TickHooks;

/**
 *  FreeRTOS expects this function to exist and requires it to be 
 *  named as such with the following signature.
 */
extern "C" void vApplicationTickHook(void);


void vApplicationTickHook(void)
{
    for (list<ApplicationTickHookFcn *>::iterator it = Ticks::TickHooks.begin();
         it != Ticks::TickHooks.end();
         it++) {

        ApplicationTickHookFcn *fcn = *it;
        (*fcn)();
    }
}

#endif


