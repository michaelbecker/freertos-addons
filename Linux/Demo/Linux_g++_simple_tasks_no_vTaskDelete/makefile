#############################################################################
# 
#   Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
# 
#   This file is part of the FreeRTOS C++ Wrappers project.
#   
#   Source Code:
#   https://github.com/michaelbecker/freertos-addons
# 
#   Project Page:
#   http://michaelbecker.github.io/freertos-addons/
# 
#   On-line Documentation:
#   http://michaelbecker.github.io/freertos-addons/docs/html/index.html
# 
#   The FreeRTOS C++ Wrappers project is free software: you can redistribute
#   it and/or modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation, either version 2
#   of the License, or (at your option) any later version.
# 
#   The FreeRTOS C++ Wrappers project is distributed in the hope that it will
#   be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#	along with the FreeRTOS C++ Wrappers project.
#   If not, see <http://www.gnu.org/licenses/>.
# 
#   NOTE: A modification to the GPL is included to allow you to distribute a
#   combined work that includes FreeRTOS C++ Wrappers project
#   without being obliged to provide the source
#   code for proprietary components.
#  
#   The FreeRTOS C++ Wrappers project GPL exception text follows:
#  
#   Any FreeRTOS C++ Wrapper *source code*, whether modified or in it's 
#   original release form, or whether in whole or in part, can only be 
#   distributed by you under the terms of the GNU General Public License plus
#   this exception.  An independent module is a module which is not derived
#   from or based on FreeRTOS C++ Wrappers project.
#  
#   Clause 1:
#  
#   Linking FreeRTOS C++ Wrappers project  with other modules is making a 
#   combined work based on FreeRTOS C++ Wrappers project. Thus, the terms 
#   and conditions of the GNU General Public License V2 cover the
#   whole combination.
#  
#   As a special exception, the copyright holders of FreeRTOS C++ Wrappers 
#   project give you permission to link FreeRTOS C++ Wrappers project with 
#   independent modules to produce a statically linked executable, regardless
#   of the license terms of these independent modules, and to copy and 
#   distribute the resulting executable under terms of your choice,
#   provided that you also meet, for each linked independent module, the 
#   terms and conditions of the license of that module.  An independent 
#   module is a module which is not derived from or based on FreeRTOS C++ 
#   Wrappers project.
#  
#   Clause 2:
#  
#   FreeRTOS C++ Wrappers project may not be used for any competitive or 
#   comparative purpose, including the publication of any form of run time 
#   or compile time metric, without the express permission of the copyright 
#   holder(s) (this is the norm within the industry and is intended to ensure
#   information accuracy).
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
CXXFLAGS += -Wall -Werror -Wextra -Wpedantic -pthread -O0 -g
LDFLAGS += -Wall -Werror -Wextra -Wpedantic -pthread -O0 -g



INCLUDE_DIRS +=	-I. \
				-I${FREERTOS_INC_DIR} \
				-I${FREERTOS_CPP_INC_DIR} \
				-I${PORTABLE_SRC_DIR} \


VPATH += ${FREERTOS_SRC_DIR} \
		 ${FREERTOS_CPP_SRC_DIR} \
		 ${PORTABLE_SRC_DIR} \
		 ${PORTABLE_SRC_MEM_MANG_DIR} \


TARGET = Linux_g++_simple_tasks_no_vTaskDelete

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


