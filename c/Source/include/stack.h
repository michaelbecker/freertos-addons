
#ifndef STACK_H_
#define STACK_H_


#include "slist.h"


/**
 *  The stack structure, we leverage low overhead singly linked lists here.
 */
typedef struct Stack_t_ {

    /**
     *  How many items are in the stack.
     */
    int Count;

    /**
     *  The head of the stack.
     */
    SListNode_t *Head;

} Stack_t;


/**
 *  Initialize a Stack structure you provide.
 *
 *  @param Stack Pointer to your stack structure.
 */
void InitStack(Stack_t *Stack);


/**
 *  Push an item onto the stack.
 *
 *  Note that you have to have embedded an SListNode inside your data 
 *  structure.
 *
 *  @param Stack Pointer to your stack structure.
 *  @param Node The SListNode you want on the stack.
 */
void PushOnStack(   Stack_t *Stack, 
                    SListNode_t *Node);


/**
 *  Pop an item off the stack.
 *
 *  Note that you have to have embedded an SListNode inside your data 
 *  structure.
 *
 *  @param Stack Pointer to your stack structure.
 *  @return An SListNode from the stack, or NULL if it's empty.
 */
SListNode_t *PopOffStack(Stack_t *Stack);


/**
 *  @return True if the stack is empty, false otherwise.
 */
#define IsStackEmpty(_stack) \
    ((_stack)->Count == 0)


#endif

