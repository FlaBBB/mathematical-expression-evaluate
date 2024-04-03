#ifndef OP_H
#define OP_H

#include <stdio.h>
#include <stdlib.h>

// Priority score
#define PLUS 1
#define MINUS 1
#define MULTIPLY 2
#define DIVIDE 2
#define MODULO 2
#define POWER 3
#define N_OPERATOR -2147483648

#define OPERAND 2
#define OPERATOR 1
#define SPACE -1

#define MIN_OPERAND 3

#define BEGIN 0
#define OPEN_BRACKET -2
#define CLOSE_BRACKET -3

#define INVALID_OP -2147483648

int is_operator(char c)
{
    switch (c)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '^':
        return 1;
    }
    return 0;
}

int is_operand(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    return 0;
}

int optype(char c)
{
    if (c == ' ')
        return SPACE;
    if (c == '(')
        return OPEN_BRACKET;
    if (c == ')')
        return CLOSE_BRACKET;
    if (is_operand(c))
        return OPERAND;
    if (is_operator)
        return OPERATOR;
    return INVALID_OP;
}

int opratscore(char c)
{
    if (!is_operator(c))
        return N_OPERATOR;
    switch (c)
    {
    case '+':
        return PLUS;
    case '-':
        return MINUS;
    case '*':
        return MULTIPLY;
    case '/':
        return DIVIDE;
    case '%':
        return MODULO;
    case '^':
        return POWER;
    }
    return -1;
}

#endif