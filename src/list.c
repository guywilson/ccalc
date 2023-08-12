#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"

int qInit(que_handle_t * hque, uint32_t size) {
    hque->pQueue = (que_item_t *)malloc(sizeof(que_item_t) * size);

    if (hque->pQueue == NULL) {
        return -1;
    }

    hque->queueLength = size;

    hque->headIndex = 0;
    hque->tailIndex = 0;
    hque->numItems = 0;

    return 0;
}

void qDestroy(que_handle_t * hque) {
    free(hque->pQueue);
}

uint32_t qGetQueLength(que_handle_t * hque) {
    return hque->queueLength;
}

uint32_t qGetNumItems(que_handle_t * hque) {
    return hque->numItems;
}

que_item_t * qGetItem(que_handle_t * hque, que_item_t * item) {
    if (hque->numItems == 0) {
        return NULL;
    }

    memcpy(item, &hque->pQueue[hque->headIndex++], sizeof(que_item_t));
    hque->numItems--;

    if (hque->headIndex == hque->queueLength) {
        hque->headIndex = 0;
    }

    return item;
}

int qPutItem(que_handle_t * hque, que_item_t item) {
    if (hque->numItems == hque->queueLength) {
        return -1;
    }

    memcpy(&hque->pQueue[hque->tailIndex++], &item, sizeof(que_item_t));
    hque->numItems++;

    if (hque->tailIndex == hque->queueLength) {
        hque->tailIndex = 0;
    }

    return 0;
}

int stackInit(stack_handle_t * hstack, int size) {
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

int stackGetLength(stack_handle_t * hstack) {
    return hstack->stackLength;
}

int stackGetNumItems(stack_handle_t * hstack) {
    return hstack->numItems;
}

stack_item_t * stackPop(stack_handle_t * hstack, stack_item_t * item) {
    if (hstack->numItems == 0) {
        return NULL;
    }

    memcpy(item, &hstack->pStack[hstack->headIndex--], sizeof(stack_item_t));
    hstack->numItems--;

    if (hstack->headIndex == -1) {
        return NULL;
    }

    return item;
}

stack_item_t * stackPeek(stack_handle_t * hstack, stack_item_t * item) {
    if (hstack->numItems == 0) {
        return NULL;
    }

    memcpy(item, &hstack->pStack[hstack->headIndex], sizeof(stack_item_t));

    return item;
}

int stackPush(stack_handle_t * hstack, stack_item_t item) {
    if (hstack->numItems == hstack->stackLength) {
        return -1;
    }

    memcpy(&hstack->pStack[hstack->headIndex++], &item, sizeof(stack_item_t));
    hstack->numItems++;

    if (hstack->headIndex == hstack->stackLength) {
        return -1;
    }

    return 0;
}
