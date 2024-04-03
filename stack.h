#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#define STACK_SIZE 1024

typedef struct
{
    void *arr[STACK_SIZE];
    int top;
} stack;

void stack_push(stack *__stack, void *__data)
{
    if (__stack->top >= STACK_SIZE)
    {
        puts("cannot push to stack, because stack is full");
        exit(-1);
    }
    __stack->arr[++__stack->top] = __data;
}

void *stack_pop(stack *__stack)
{
    if (__stack->top < 0)
    {
        puts("cannot pop stack, because stack is empty");
        exit(-1);
    }
    return __stack->arr[__stack->top--];
}

#endif // STACK_H