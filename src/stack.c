#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "stack.h"

int stackInit(stack_handle_t * hstack, uint32_t size) {
    hstack->pStack = (stack_item_t *)malloc(sizeof(stack_item_t) * size);

    if (hstack->pStack == NULL) {
        return -1;
    }

    hstack->stackLength = size;

    hstack->headIndex = 0;
    hstack->tailIndex = 0;
    hstack->numItems = 0;

    return 0;
}

void stackDestroy(stack_handle_t * hstack) {
    free(hstack->pStack);
}

uint32_t stackGetLength(stack_handle_t * hstack) {
    return hstack->stackLength;
}

uint32_t stackGetNumItems(stack_handle_t * hstack) {
    return hstack->numItems;
}

stack_item_t * stackPop(stack_handle_t * hstack, stack_item_t * item) {
    if (hstack->numItems == 0) {
        return NULL;
    }

    memcpy(item, &hstack->pStack[hstack->headIndex++], sizeof(stack_item_t));
    hstack->numItems--;

    if (hstack->headIndex == hstack->stackLength) {
        hstack->headIndex = 0;
    }

    return item;
}

int stackPush(stack_handle_t * hstack, stack_item_t item) {
    if (hstack->numItems == hstack->stackLength) {
        return -1;
    }

    memcpy(&hstack->pStack[hstack->tailIndex++], &item, sizeof(stack_item_t));
    hstack->numItems++;

    if (hstack->tailIndex == hstack->stackLength) {
        hstack->tailIndex = 0;
    }

    return 0;
}
