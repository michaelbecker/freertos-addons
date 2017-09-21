
#ifndef QUEUE_H_
#define QUEUE_H_


#include "dlist.h"


typedef struct Queue_t_ {

    /**
     *  How many items are in the queue.
     */
    int Count;

    /**
     *  We use a doubly linked list as our queue.
     */
    DlNode_t Head;

} Queue_t;


void InitQueue(Queue_t *Queue);


void Enqueue(Queue_t *Queue, DlNode_t *Node);


DlNode_t *Dequeue(Queue_t *Queue);


/**
 *  @return True if the stack is empty, false otherwise.
 */
#define IsQueueEmpty(_queue) \
    ((_queue)->Count == 0)


#endif


