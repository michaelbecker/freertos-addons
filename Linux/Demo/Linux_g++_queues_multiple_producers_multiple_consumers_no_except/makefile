#############################################################################
#
#  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
#
#  This file is part of the FreeRTOS Add-ons project.
#
#  Source Code:
#  https://github.com/michaelbecker/freertos-addons
#
#  Project Page:
#  http://michaelbecker.github.io/freertos-addons/
#
#  On-line Documentation:
#  http://michaelbecker.github.io/freertos-addons/docs/html/index.html
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so,subject to the following conditions:
#
#  + The above copyright notice and this permission notice shall be included
#    in all copies or substantial portions of the Software.
#  + Credit is appreciated, but not required, if you find this project useful
#    enough to include in your application, product, device, etc.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#
#############################################################################


FREERTOS_SRC_DIR=../../../../FreeRTOS/Source
FREERTOS_INC_DIR=../../../../FreeRTOS/Source/include
PORTABLE_SRC_DIR=../../portable/GCC/Linux
PORTABLE_SRC_MEM_MANG_DIR=../../../../FreeRTOS/Source/portable/MemMang
FREERTOS_CPP_SRC_DIR=../../../c++/Source/
FREERTOS_CPP_INC_DIR=../../../c++/Source/include


CC = gcc
CXX = g++

CFLAGS += -Wall -Werror -Wextra -Wpedantic -pthread -O0 -g
CXXFLAGS += -Wall -Werror -Wextra -Wpedantic -pthread -O0 -g -DCPP_FREERTOS_NO_EXCEPTIONS
LDFLAGS += -Wall -Werror -Wextra -Wpedantic -pthread -O0 -g



INCLUDE_DIRS +=	-I. \
				-I${FREERTOS_INC_DIR} \
				-I${FREERTOS_CPP_INC_DIR} \
				-I${PORTABLE_SRC_DIR} \


VPATH += ${FREERTOS_SRC_DIR} \
		 ${FREERTOS_CPP_SRC_DIR} \
		 ${PORTABLE_SRC_DIR} \
		 ${PORTABLE_SRC_MEM_MANG_DIR} \


TARGET = Linux_g++_queues_multiple_producers_multiple_consumers_no_except

default: all

all: ${TARGET}

SRC = \
	  main.cpp

FREERTOS_SRC= \
			  event_groups.c \
			  list.c \
			  queue.c \
			  tasks.c \
			  timers.c \
			  port.c \
			  heap_3.c \
			  croutine.c \


FREERTOS_CPP_SRC= \
				  cmutex.cpp \
				  cqueue.cpp \
				  cread_write_lock.cpp \
				  csemaphore.cpp \
				  ctasklet.cpp \
				  cthread.cpp \
				  ctimer.cpp \
				  ctickhook.cpp \
				  cworkqueue.cpp \


OBJ = ${SRC:.cpp=.o} 
${OBJ}: %.o: %.cpp
	${CXX} ${INCLUDE_DIRS} ${CXXFLAGS} -c $< -o $@

FREERTOS_OJB = ${FREERTOS_SRC:.c=.o} 
${FREERTOS_OJB}: %.o: %.c
	${CC} -I. -I${FREERTOS_INC_DIR} -I${PORTABLE_SRC_DIR} ${CFLAGS} -c $< -o $@

FREERTOS_CPP_OJB = ${FREERTOS_CPP_SRC:.cpp=.o} 
${FREERTOS_CPP_OJB}: %.o: %.cpp
	${CXX} ${INCLUDE_DIRS} ${CXXFLAGS} -c $< -o $@

$(TARGET):	${FREERTOS_OJB} ${FREERTOS_CPP_OJB} ${OBJ}
	${CXX} ${LDFLAGS} ${LD_EXE_FLAGS} -o $@ ${FREERTOS_CPP_OJB} ${FREERTOS_OJB} ${OBJ} ${LIBS} 


#$(TARGET):	${FREERTOS_OJB}
#		${CXX} ${INCLUDE_DIRS} ${CPPFLAGS} ${CPP_EXE_FLAGS} -c $? 
#		${CXX} ${LDFLAGS} ${LD_EXE_FLAGS} -o $@ ${OBJ} ${FREERTOS_CPP_OJB} ${FREERTOS_OJB} ${LIBS} 

.PHONY : clean
clean:
	-rm ${OBJ}
	-rm *~
	-rm ${TARGET}
	-rm *.o


