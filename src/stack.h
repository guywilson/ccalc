#include <stdint.h>
#include <stdbool.h>

#ifndef __INCL_QUE
#define __INCL_QUE

typedef struct {
    void *          item;
    uint32_t        itemLength;
}
stack_item_t;

struct _stack_handle_t {
    stack_item_t *  pStack;

    uint32_t        stackLength;
    uint32_t        numItems;

    int             headIndex;
    int             tailIndex;
};

typedef struct _stack_handle_t            stack_handle_t;

int             stackInit(stack_handle_t * hque, uint32_t size);
void            stackDestroy(stack_handle_t * hque);
uint32_t        stackGetLength(stack_handle_t * hque);
uint32_t        stackGetNumItems(stack_handle_t * hque);
stack_item_t *  stackPop(stack_handle_t * hque, stack_item_t * item);
int             stackPush(stack_handle_t * hque, stack_item_t item);

#endif
