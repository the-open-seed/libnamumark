//
// Created by hibot on 23/08/24.
//
#include "stack.h"

#include <stdlib.h>
// stack functions
stack *stack_new(void) {
    stack *new_stack = malloc(sizeof(stack));
    // check if malloc failed
    if (new_stack == NULL) {
        abort();
    }
    new_stack->data = NULL;
    new_stack->size = 0;
    return new_stack;
}

void stack_push(stack *stack, void *data) {
    void **new_data = realloc(stack->data, sizeof(void *) * (stack->size + 1));
    // check if realloc failed
    if (new_data == NULL) {
        abort();
    }
    new_data[stack->size] = data;
    stack->data = new_data;
    stack->size++;
}

void *stack_pop(stack *stack) {
    void *data = stack->data[stack->size - 1];
    void **new_data = realloc(stack->data, sizeof(void *) * (stack->size - 1));
    // check if realloc failed
    if (new_data == NULL) {
        abort();
    }
    stack->data = new_data;
    stack->size--;
    return data;
}