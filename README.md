# freertos-addons

After working with FreeRTOS for over 6 years now, I've decided to start adding features and implementations that I wish would have been there at the beginning. 

## Current Features

+ C++ Wrappers [![Coverity Scan Build Status](https://scan.coverity.com/projects/9669/badge.svg)](https://scan.coverity.com/projects/michaelbecker-freertos-addons)
  - Licensing is GPLv2. Anyone is free to use this anywhere they want however they want, with the understanding that their code will also be required to be GPLv2, and open to the public. However, if alternate licensing is required or desired, please contact me. 
  - A collection of C++ wrappers encapsulating FreeRTOS functionality, allowing you to write your RTOS application in C++ while still using FreeRTOS. This wrapper layer does all the integration work for you.
  - This library is for you if you are planning on using C++ and FreeRTOS in your project but don't want to spend the time integrating the two.
  - Everything was testing successfully using FreeRTOS versions 8.2.3 and 9.0.0.
  - There are numerous demo / unit test projects using these wrappers and various features they provide. (Last count we are at 24 Demo projects showing how you might use the C++ Wrapper library.
  - [Project web page](http://michaelbecker.github.io/freertos-addons/)
  - [Full cross-referenced documentation](http://michaelbecker.github.io/freertos-addons/docs/html/index.html)

+ Updated Linux port
  - Licensing is GPLv2 (this is a derivative work).
  - An update of the Linux / POSIX port of FreeRTOS, for testing and debugging on a Linux PC.
  - This update was tested successfully with FreeRTOS 8.2.3 and 9.0.0 
  - Two Ubuntu workstations were also used, running 16.04 and 14.04, both 64-bit multicore machines. 
  - This revised Posix port I renamed to Linux port, since I ended up using a few Linux specific APIs in it.
  - Thanks to William Davy who wrote the original Posix FreeRTOS port. He did amazing work simulating a real-time system under Linux.


## Releases

### Version 1.1.0 
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.1.0
+ Adding object oriented work queues.
+ Coverity scan analysis performed, with bug fixes.
  - See: https://scan.coverity.com/projects/michaelbecker-freertos-addons

### Version 1.0.2
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.0.2
+ Fixed a major bug in Thread creation.
  - See: https://www.linkedin.com/pulse/apples-kumquats-perils-multi-threaded-polymorphism-michael-becker

### Version 1.0.1
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.0.1
+ Changed license from GPLv3 to GPLv2 (or later) to accomodate FreeRTOS's modified GPLv2 license.
+ See https://www.gnu.org/licenses/gpl-faq.html#AllCompatibility for more information.

### Version 1.0.0 
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.0.0
+ Initial release.

## TODO

This is my todo list for this project. If there's something you'd like to see done sooner, feel free to make a request.

### FreeRTOS Features Not Yet Implemented
+ events
+ MPU restricted threads
+ eTaskGetState
+ uxTaskGetNumberOfTasks
+ uxTaskGetStackHighWaterMark
+ Thread Local Storage

### New Value Added
+ In Timers, add OnStop / OnStart / mutex sync with these methods?
+ Fixed size Memory pools
+ Priority queues
+ Monitors 
+ Condition Variables
+ Tick Hooks - option to round robin hooks, one per tick


