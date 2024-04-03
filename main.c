#include "stack.h"
#include "op.h"

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include <math.h>
#include <errno.h>

#define BUFF_SIZE 2048
#define MAX_DIGIT_OPERAND 32

void print_exp(const char *expression, size_t exp_size, size_t idx, size_t depth)
{
    printf("expression error: '");
    size_t start = MAX((int)0, (int)(idx - depth));
    size_t end = MIN((int)exp_size, (int)(idx + depth));
    for (size_t i = start; i <= end; i++)
    {
        if (i == idx)
            printf("\e[0;31m");
        printf("%c", expression[i]);
        if (i == idx)
            printf("\e[0m");
    }
    printf("'\n");

    for (size_t i = start; i < idx + 19; i++)
    {
        printf(" ");
    }
    printf("\e[0;31m^\e[0m\n");
    return;
}

size_t infix_to_postfix(const char *expression, size_t exp_size, char **postfix, size_t postfix_size)
{
    // make sure postfix_size is enough for storing data
    if (exp_size > postfix_size)
    {
        printf("postfix size is must be equal or more than expression size\n");
        errno = 1;
        return 0;
    }
    char c, _c;

    // idx of expression
    int idx_exp = 0;

    // intialization the operator stack
    stack operator_stack;
    operator_stack.top = -1;

    // intialization the index
    size_t postfix_idx = 0;

    char operand_remainder[MAX_DIGIT_OPERAND] = {0};
    size_t operand_r_idx = 0;

    int lchar_optype = BEGIN;
    int last_optype = BEGIN;

    while (idx_exp < exp_size)
    {
        c = expression[idx_exp];

        // check type of c, make sure it is valid token
        int c_optype = optype(c);

        if (c_optype == INVALID_OP)
        {
            printf("'%c' is not a valid expression\n", c);
            print_exp(expression, exp_size, idx_exp, 3);
            errno = 1;
            return 0;
        }

        // get all operand until not find operand again
        if (c_optype == OPERAND)
        {
            if (last_optype == OPERAND)
            {
                printf("the syntax is invalid (operand meet with operand)\n", c);
                print_exp(expression, exp_size, idx_exp, 3);
                errno = 1;
                return 0;
            }

            if (operand_r_idx == MAX_DIGIT_OPERAND)
            {
                printf("operand is limited with %d digits\n", MAX_DIGIT_OPERAND);
                print_exp(expression, exp_size, idx_exp, 3);
                errno = 1;
                return 0;
            }

            operand_remainder[operand_r_idx++] = c;
            lchar_optype = c_optype;
            goto END_ITP_LOOP;
        }
        else
        {
            if (operand_r_idx > 0)
            {
                if (last_optype == MIN_OPERAND)
                {
                    postfix[postfix_idx] = malloc(operand_r_idx + 1);
                    memcpy(postfix[postfix_idx], "-", operand_r_idx);
                    memcpy(&postfix[postfix_idx++][1], operand_remainder, operand_r_idx);
                }
                else
                {
                    postfix[postfix_idx] = malloc(operand_r_idx);
                    memcpy(postfix[postfix_idx++], operand_remainder, operand_r_idx);
                }

                memset(operand_remainder, 0, MAX_DIGIT_OPERAND);
                operand_r_idx = 0;
                last_optype = lchar_optype;
            }
            else if (last_optype == MIN_OPERAND)
            {
                printf("syntax error, '-' must be followed by operand\n");
                print_exp(expression, exp_size, idx_exp, 3);
                errno = 1;
                return 0;
            }
        }

        if (c == ')')
        {
            if (last_optype != OPERAND)
            {
                printf("syntax is invalid\n");
                print_exp(expression, exp_size, idx_exp, 3);
                errno = 1;
                return 0;
            }

            while (operator_stack.top > 0 && (char)(intptr_t)operator_stack.arr[operator_stack.top] != '(')
            {
                _c = (char)(intptr_t)stack_pop(&operator_stack);
                postfix[postfix_idx] = malloc(1);
                memcpy(postfix[postfix_idx++], &_c, 1);
            }

            if ((char)(intptr_t)operator_stack.arr[operator_stack.top] != '(')
            {
                printf("syntax error, open bracket '(' is missing\n");
                print_exp(expression, exp_size, idx_exp, 3);
                errno = 1;
                return 0;
            }

            stack_pop(&operator_stack);
            last_optype = CLOSE_BRACKET;
            goto END_ITP_LOOP;
        }

        if (c == '(')
        {
            if (last_optype != OPERATOR)
            {
                printf("syntax is invalid\n");
                print_exp(expression, exp_size, idx_exp, 3);
                errno = 1;
                return 0;
            }

            stack_push(&operator_stack, (void *)(intptr_t)c);
            last_optype = OPEN_BRACKET;
        }

        if (c_optype == OPERATOR)
        {
            if (last_optype == OPERATOR || last_optype == BEGIN || last_optype == OPEN_BRACKET || last_optype == MIN_OPERAND)
            {
                if (c != '-' || last_optype == MIN_OPERAND)
                {
                    printf("syntax is invalid with token '%c'\n", c);
                    print_exp(expression, exp_size, idx_exp, 3);
                    errno = 1;
                    return 0;
                }
                last_optype = MIN_OPERAND;
                goto END_ITP_LOOP;
            }

            int op_i = opratscore(c);
            while (operator_stack.top >= 0 && opratscore((char)(intptr_t)operator_stack.arr[operator_stack.top]) >= op_i)
            {
                _c = (char)(intptr_t)stack_pop(&operator_stack);
                postfix[postfix_idx] = malloc(1);
                memcpy(postfix[postfix_idx++], &_c, 1);
            }
            stack_push(&operator_stack, (void *)(intptr_t)c);
            last_optype = OPERATOR;
        }

    END_ITP_LOOP:
        idx_exp++;
    }

    if (operand_r_idx > 0)
    {
        if (last_optype == MIN_OPERAND)
        {
            postfix[postfix_idx] = malloc(operand_r_idx + 1);
            memcpy(postfix[postfix_idx], "-", operand_r_idx);
            memcpy(&postfix[postfix_idx++][1], operand_remainder, operand_r_idx);
        }
        else
        {
            postfix[postfix_idx] = malloc(operand_r_idx);
            memcpy(postfix[postfix_idx++], operand_remainder, operand_r_idx);
        }

        memset(operand_remainder, 0, MAX_DIGIT_OPERAND);
        operand_r_idx = 0;
        last_optype = lchar_optype;
    }

    while (operator_stack.top >= 0)
    {
        _c = (char)(intptr_t)stack_pop(&operator_stack);
        if (_c == '(')
        {
            printf("syntax error, close bracket is missing ')'\n");
            errno = 1;
            return 0;
        }

        postfix[postfix_idx] = malloc(1);
        memcpy(postfix[postfix_idx++], &_c, 1);
    }

    return postfix_idx;
}

long do_operation(char c, long a, long b)
{
    switch (c)
    {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        return a / b;
    case '%':
        return a % b;
    case '^':
        return powl(a, b);
    default:
        printf("something wrong (operation doesn't exists)\n");
        errno = 1;
        return 0;
    }
}

long evaluate_postfix(char **postfix, size_t l_postfix)
{
    stack operand_stack;
    operand_stack.top = -1;

    size_t postfix_idx = 0;

    char *token;
    size_t l_token;

    long operand;

    while (postfix_idx < l_postfix)
    {
        token = postfix[postfix_idx++];
        l_token = strlen(token);

        if (is_operand(token[l_token - 1]))
        {
            operand = strtol(token, NULL, 10);

            if (errno != 0)
            {
                perror("strtol");
                errno = 1;
                return 0;
            }

            stack_push(&operand_stack, (void *)operand);
        }
        else
        {
            if (operand_stack.top < 1)
            {
                printf("syntax error (not enough operand)\n");
                errno = 1;
                return 0;
            }

            operand = (long)stack_pop(&operand_stack);
            operand = do_operation(token[0], (long)operand_stack.arr[operand_stack.top], operand);

            if (errno != 0)
                return 0;

            operand_stack.arr[operand_stack.top] = (void *)operand;
        }
    }

    if (operand_stack.top != 0)
    {
        printf("syntax error (not enough operator)\n");
        errno = 1;
        return 0;
    }
    return (long)stack_pop(&operand_stack);
}

void eval_expression(const char *expression, size_t l_expression)
{
    char *postfix[l_expression];
    size_t l_postfix = infix_to_postfix(expression, l_expression, postfix, l_expression);
    if (errno != 0)
        return;

    printf("expression: %s", expression);
    for (size_t i = 0; i < l_postfix; i++)
    {
        printf("%lu: '%s'\n", i, postfix[i]);
    }

    long result = evaluate_postfix(postfix, l_postfix);
    if (errno != 0)
        return;

    printf("result: %d\n\n", result);
}

int main(int argc, char const *argv[])
{
    char exp[BUFF_SIZE];
    size_t l_exp;

    while (1)
    {
        errno = 0;
        printf("> ");
        fgets(exp, BUFF_SIZE, stdin);

        l_exp = strlen(exp) - 1;

        eval_expression((const char *)exp, l_exp);
    }

    return 0;
}