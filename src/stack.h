//
// Created by hibot on 23/08/24.
//

#ifndef LIBNAMUMARK_STACK_H
#ifdef __cplusplus
extern "C" {
#endif
#define LIBNAMUMARK_STACK_H
struct stack {
    void **data;
    int size;
};
typedef struct stack stack;
stack *stack_new(void);
void stack_push(stack *stack, void *data);
void *stack_pop(stack *stack);
#ifdef __cplusplus
}
#endif
#endif //LIBNAMUMARK_STACK_H
