#include <stdint.h>
#include <stdbool.h>

#ifndef __INCL_LIST
#define __INCL_LIST

typedef struct {
    void *          item;
    uint32_t        itemLength;
}
list_item_t;

struct _stack_handle_t {
    list_item_t *   pStack;

    int             stackLength;
    int             numItems;

    int             headIndex;      // The stack pointer...
    int             tailIndex;
};

typedef struct _stack_handle_t            stack_handle_t;

struct _que_handle_t {
    list_item_t *   pQueue;

    uint32_t        queueLength;
    uint32_t        numItems;

    int             headIndex;
    int             tailIndex;
};

typedef struct _que_handle_t            que_handle_t;

int             stackInit(stack_handle_t * hque, int size);
void            stackDestroy(stack_handle_t * hque);
int             stackGetLength(stack_handle_t * hque);
int             stackGetNumItems(stack_handle_t * hque);
list_item_t *   stackPop(stack_handle_t * hque, list_item_t * item);
list_item_t *   stackPeek(stack_handle_t * hstack, list_item_t * item);
int             stackPush(stack_handle_t * hque, list_item_t item);

int             qInit(que_handle_t * hque, uint32_t size);
void            qDestroy(que_handle_t * hque);
uint32_t        qGetQueLength(que_handle_t * hque);
uint32_t        qGetNumItems(que_handle_t * hque);
list_item_t *   qGetItem(que_handle_t * hque, list_item_t * item);
int             qPutItem(que_handle_t * hque, list_item_t item);

#endif
