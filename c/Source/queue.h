

#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct Queue_t_ {

    /**
     *  How many items are in the queue.
     */
    int Count;

    /**
     *  The head of the stack.
     */
    SListNode_t *Head;

    /**
     *  The tail of the stack.
     */
    SListNode_t *Tail;

} Queue_t;



#endif

