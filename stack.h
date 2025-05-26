#include <stdbool.h>

#ifndef STACK_HEADER
#define STACK_HEADER

typedef struct Stack {
    int size;
    void **content;
} Stack;

Stack *stack_new();

void stack_print(Stack *stack);
void stack_free(Stack *stack);
void stack_push(Stack *stack, void *item);
void *stack_pop(Stack *stack);
void *stack_peek(Stack *stack);
bool stack_empty(Stack *stack);

#endif