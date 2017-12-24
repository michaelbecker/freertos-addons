# freertos-addons

After working with FreeRTOS for over 7 years now, I've decided to start adding features and implementations that I wish would have been there at the beginning. 

## Current Features
+ C Add-on Wrappers
  - A collection of C Add-on functionality for FreeRTOS. Right now these consist of:
  - Memory Pools: Fixed size memory allocation buffers. Using these elminates the possibility of memory fragmentation. There is overhead associated with these, so it's better if you are maximizing the size of each allocation.
  - Reader / Writer Locks: These allow multiple threads to simultaneously access a shared resource all as readers. If something needs to change, then a Writer lock needs to be taken which will allow a singe thread to modify the shared resource.
  - Workqueues: These allow you to queue "work" (i.e. a function) to a different thread. Useful if you have a lot of "one off" things that need to be done in different threads but they happen very asynchronous.
  - In addition, to support these there are implementations of standard optimized compter science singly linked lists, doubly linked circular lists, queues, and stacks.
  - [Full cross-referenced documentation](http://michaelbecker.github.io/freertos-addons/cdocs/html/index.html). Documents were auto-generated and cross-referenced using Doxygen.

+ C++ Wrappers [![Coverity Scan Build Status](https://scan.coverity.com/projects/9669/badge.svg)](https://scan.coverity.com/projects/michaelbecker-freertos-addons)
  - A collection of C++ wrappers encapsulating FreeRTOS functionality, allowing you to write your RTOS application in C++ while still using FreeRTOS. This wrapper layer does all the integration work for you.
  - This library is for you if you are planning on using C++ and FreeRTOS in your project but don't want to spend the time integrating the two.
  - Everything was testing successfully using FreeRTOS versions 8.2.3 and 9.0.0.
  - There are numerous demo / unit test projects using these wrappers and various features they provide. (Last count we are at 42 Demo projects showing how you might use the C++ Wrapper library.
  - Licensing is a modified GPLv2, the same as FreeRTOS.
  - [Project web page](http://michaelbecker.github.io/freertos-addons/)
  - [Full cross-referenced documentation](http://michaelbecker.github.io/freertos-addons/cppdocs/html/index.html). Documents were auto-generated and cross-referenced using Doxygen.

+ Updated Linux port
  - Licensing is GPLv2 (this is a derivative work).
  - An update of the Linux / POSIX port of FreeRTOS, for testing and debugging on a Linux PC.
  - This update was tested successfully with FreeRTOS 8.2.3 and 9.0.0 
  - Two Ubuntu workstations were also used, running 16.04 and 14.04, both 64-bit multicore machines. 
  - This revised Posix port I renamed to Linux port, since I ended up using a few Linux specific APIs in it.
  - Thanks to William Davy who wrote the original Posix FreeRTOS port. He did amazing work simulating a real-time system under Linux.


## Overall Releases

### Version 1.5.1
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.5.1
+ C++ Wrappers 1.5.0:
  - Added alignment requirements to memory pools.
  - Updated doxygen documentation.
+ C Add-ons 1.0.1:
  - Added static allocated fixed size memory pools.
  - Updated doxygen documentation.

### Version 1.5.0
+ Starting to independently version the overall project vs the unique packages within it.
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.5.0
+ Added new major feature, C Add-ons. This is version 1.0.0 of the C Add-ons.
+ C Add-ons include:
  - Fixed size memory pools
  - Read / Write locks
  - Work queues

### Version 1.4.0
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.4.0
+ Added new feature, fixed size memory pools and associated unit tests.

### Version 1.3.2
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.3.2
+ Bug fixes to 1.3.1 regarding the critical section wrapper. Code prior to this version in critical.hpp will not compile successfully. 
+ Added a Demo project using straight C and the updated Linux simulator only.

### Version 1.3.1
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.3.1
+ Bug fixes to 1.3.0, related to defining vTaskDelete to 0, as well as needing to make the Timer class's dtor virtual. Many thanks to Ewout Boks from the HAN technical college, Arnhem, the Netherlands for finding these.

### Version 1.3.0
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.3.0
+ Added preprocessor variable "CPP_FREERTOS_NO_CPP_STRINGS". If you do not want to use C++ strings, simply define this in your makefile or project. Note that if you define this, you must also define "CPP_FREERTOS_NO_EXCEPTIONS". Some classes throw exceptions if they cannot be constructed, and the exceptions they throw depend of C++ strings.
+ Added an implementation of condition variables. These are not enabled by default. To use, define "CPP_FREERTOS_CONDITION_VARIABLES". Also include the files condition_variable.hpp and ccondition_variable.cpp in your project.

### Version 1.2.0
+ https://github.com/michaelbecker/freertos-addons/releases/tag/v1.2.0
+ Adding preprocessor flag to exclude exceptions for smaller footprint and C++ compilers that do not support them.
+ Updated license to one similar to FreeRTOS version. What does that mean for you? It means you are free to use FreeRTOS C++ Wrappers in your commercial product without making your product open source.

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
+ Priority queues
+ Tick Hooks - option to round robin hooks, one per tick


