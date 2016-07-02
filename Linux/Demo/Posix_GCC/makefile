
FREERTOS_SRC_DIR=../../../../FreeRTOS/Source
FREERTOS_INC_DIR=../../../../FreeRTOS/Source/include
PORTABLE_SRC_DIR=../../portable/GCC/Posix
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


TARGET = Posix_Test

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
				  ctickhook.cpp


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


