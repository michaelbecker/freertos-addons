
FREERTOS_SRC_DIR=../../../../FreeRTOS/Source
FREERTOS_INC_DIR=../../../../FreeRTOS/Source/include
PORTABLE_SRC_DIR=../../portable/GCC/null
FREERTOS_CPP_SRC_DIR=../../../c++/Source/
FREERTOS_CPP_INC_DIR=../../../c++/Source/include


CC = gcc
CXX = g++

CFLAGS += -Wall -Werror 

INCLUDE_DIRS +=	-I. \
				-I${FREERTOS_INC_DIR} \
				-I${FREERTOS_CPP_INC_DIR} \
				-I${PORTABLE_SRC_DIR} \


VPATH += ${FREERTOS_SRC_DIR} \
		 ${FREERTOS_CPP_SRC_DIR} \
		 ${PORTABLE_SRC_DIR}


TARGET = null_Test

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


FREERTOS_CPP_SRC= \
				  mutex.cpp \
				  queue.cpp \
				  read_write_lock.cpp \
				  semaphore.cpp \
				  tasklet.cpp \
				  thread.cpp \
				  timer.cpp


OBJ = ${SRC:.cpp=.o} 

FREERTOS_OJB = ${FREERTOS_SRC:.c=.o} 

${FREERTOS_OJB}: %.o: %.c
	${CC} ${INCLUDE_DIRS} ${CFLAGS} -c $< -o $@

FREERTOS_CPP_OJB = ${FREERTOS_CPP_SRC:.cpp=.o} 

$(TARGET):	${FREERTOS_OJB} ${FREERTOS_CPP_OJB} ${OBJ}


#$(TARGET):	${FREERTOS_OJB}
#		${CXX} ${INCLUDE_DIRS} ${CPPFLAGS} ${CPP_EXE_FLAGS} -c $? 
#		${CXX} ${LDFLAGS} ${LD_EXE_FLAGS} -o $@ ${OBJ} ${FREERTOS_CPP_OJB} ${FREERTOS_OJB} ${LIBS} 

.PHONY : clean
clean:
	-rm ${OBJ}
	-rm *~
	-rm ${TARGET}
	-rm *.o


