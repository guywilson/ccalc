#include <stdint.h>
#include <stdbool.h>

#ifndef __INCL_STACK
#define __INCL_STACK

typedef struct {
    void *          item;
    uint32_t        itemLength;
}
stack_item_t;

struct _stack_handle_t {
    stack_item_t *  pStack;

    int             stackLength;
    int             numItems;

    int             headIndex;      // The stack pointer...
    int             tailIndex;
};

typedef struct _stack_handle_t            stack_handle_t;

int             stackInit(stack_handle_t * hque, int size);
void            stackDestroy(stack_handle_t * hque);
int             stackGetLength(stack_handle_t * hque);
int             stackGetNumItems(stack_handle_t * hque);
stack_item_t *  stackPop(stack_handle_t * hque, stack_item_t * item);
int             stackPush(stack_handle_t * hque, stack_item_t item);

#endif
