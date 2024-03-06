#############################################################################
#
#  Copyright (c) 2023, Michael Becker (michael.f.becker@gmail.com)
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
#  MIT License
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


FREERTOS_SRC_DIR=../../../../FreeRTOS-Kernel
FREERTOS_INC_DIR=../../../../FreeRTOS-Kernel/include
PORTABLE_SRC_MEM_MANG_DIR=../../../../FreeRTOS-Kernel/portable/MemMang
PORTABLE_SRC_DIR=../../../../FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix
PORTABLE_SRC_DIR_UTILS=../../../../FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix/utils
FREERTOS_C_ADDONS_SRC_DIR=../../../c/Source/
FREERTOS_C_ADDONS_INC_DIR=../../../c/Source/include


CC = gcc

CFLAGS += -Wall -Werror -Wextra -Wpedantic -pthread -O0 -g
LDFLAGS += -Wall -Werror -Wextra -Wpedantic -pthread -O0 -g



INCLUDE_DIRS +=	-I. \
				-I${FREERTOS_INC_DIR} \
				-I${FREERTOS_C_ADDONS_INC_DIR} \
				-I${PORTABLE_SRC_DIR} \
				-I${PORTABLE_SRC_DIR_UTILS}} \


VPATH += ${FREERTOS_SRC_DIR} \
		 ${FREERTOS_C_ADDONS_SRC_DIR} \
		 ${PORTABLE_SRC_DIR} \
		 ${PORTABLE_SRC_DIR_UTILS} \
		 ${PORTABLE_SRC_MEM_MANG_DIR} \


default: all

all: ${TARGET}


FREERTOS_SRC+= \
			  event_groups.c \
			  list.c \
			  queue.c \
			  tasks.c \
			  timers.c \
			  port.c \
			  heap_3.c \
			  wait_for_event.c \


FREERTOS_C_ADDONS_SRC+= \
					slist.c \
					stack_simple.c \
				  	mem_pool.c \


OBJ = ${SRC:.c=.o}
${OBJ}: %.o: %.c
	${CC} ${INCLUDE_DIRS} ${CFLAGS} -c $< -o $@

FREERTOS_OJB = ${FREERTOS_SRC:.c=.o} 
${FREERTOS_OJB}: %.o: %.c
	${CC} -I. -I${FREERTOS_INC_DIR} -I${PORTABLE_SRC_DIR} ${CFLAGS} -c $< -o $@


FREERTOS_C_ADDONS_OBJ = ${FREERTOS_C_ADDONS_SRC:.c=.o}
${FREERTOS_C_ADDONS_OBJ}: %.o: %.c
	${CC} ${INCLUDE_DIRS} ${CFLAGS} -c $< -o $@


$(TARGET):	${FREERTOS_OJB} ${FREERTOS_C_ADDONS_OBJ} ${OBJ}
	${CC} ${LDFLAGS} ${LD_EXE_FLAGS} -o $@ ${FREERTOS_C_ADDONS_OBJ} ${FREERTOS_OJB} ${OBJ} ${LIBS} 


.PHONY : clean
clean:
	-rm ${OBJ}
	-rm *~
	-rm ${TARGET}
	-rm *.o

# vim: filetype=make
