/****************************************************************************
 *
 ***************************************************************************/
#ifndef QUEUE_HPP_
#define QUEUE_HPP_


#include <exception>
#include <string>
#include "FreeRTOS.h"
#include "queue.h"


namespace cpp_freertos {


/**
 *  This is the exception that is thrown if a Queue constructor fails.
 */
class QueueCreateException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        QueueCreateException()
        {
            sprintf(errorString, "Queue Constructor Failed");
        }

        /**
         *  Create the exception.
         */
        QueueCreateException(const char *info)
        {
            snprintf(errorString, sizeof(errorString),
                        "Queue Constructor Failed %s", info);
        }

        /**
         *  Get what happened as a string.
         *  We are overriding the base implementation here.
         */
        virtual const char *what() const throw()
        {
            return errorString;
        }

    private:
        /**
         *  A text string representing what failed.
         */
        char errorString[80];
};


class Queue {

    public:
        Queue(UBaseType_t maxItems, UBaseType_t itemSize);
        virtual ~Queue();

        virtual bool Enqueue(void *item);
        virtual bool Enqueue(void *item, TickType_t xBlockTime);
        bool Dequeue(void *item, TickType_t xBlockTime = portMAX_DELAY);
        bool Peek(void *item, TickType_t xBlockTime = portMAX_DELAY);

        virtual bool EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken);
        bool DequeueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken);
        bool PeekFromISR(void *item);

        bool IsEmpty();
        bool IsFull();
        void Flush();

        UBaseType_t NumItems();
        UBaseType_t NumSpacesLeft();

    protected:
        QueueHandle_t handle;
};


/**
 *  Double ended queue (almost).
 */
class CDeque : public Queue {

    public:
        CDeque(UBaseType_t maxItems, UBaseType_t itemSize);
        bool EnqueueToFront(void *item, TickType_t xBlockTime = portMAX_DELAY);
        bool EnqueueToFrontFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken);
};


/**
 *  Binary queue with overwrite.
 */
class CBinaryQueue : public Queue {

    public:

        CBinaryQueue(UBaseType_t itemSize);

        /**
         *  @return true always, because of overwrite.
         */
        virtual bool Enqueue(void *item);

        /**
         *  @return true always, because of overwrite.
         */
        virtual bool EnqueueFromISR(void *item, BaseType_t *pxHigherPriorityTaskWoken);
};


}
#endif
