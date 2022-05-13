#include "calc.h"
#include <locale.h>

char* calculate(char *expr) {
    setlocale(LC_ALL, "en_US.utf8");
    char *rpn_expr = convert_expr(expr);
    char *res = NULL;
    if (rpn_expr) {
        res = make_calculation(rpn_expr);
        free(rpn_expr);
    }
    return res;
}

char* make_calculation(char *expr) {
    char *res = (char*)calloc(NUM_MAX_LENGTH, sizeof(char));
    double res_number = 0;
    int len = strlen(expr);
    int mant_length = 0;
    int func_len = 0;
    my_stack_t *number_my_stack = init_my_stack(STACK_DOUBLE);
    while (*expr) {
        if (*expr != ' ') {
            if (is_number(*expr))
                process_number(&expr, &number_my_stack, len, &mant_length);
            else if (is_binary_operator(*expr) || *expr == '%')
                process_operator(&number_my_stack, &expr);
            else if ((func_len = is_function_in_expr(expr)))
                process_function(&expr, &number_my_stack, func_len);
        } else {
            expr += 1;
        }
    }
    pop(&number_my_stack, &res_number);
    int num_err = 0;
    if (number_my_stack->next != NULL || isinf(res_number) || res_number == -1.0 / 0.0) {
        free(res);
        res = NULL;
        num_err = 1;
    }
    if (!num_err) {
        if (mant_length != 0 || func_len)
            mant_length = 7;
        snprintf(res, NUM_MAX_LENGTH, "%.*lf", mant_length, res_number);
    }
    destroy_my_stack(number_my_stack);
    return res;
}

void process_number(char **expr, my_stack_t **my_stack, int len, int *mant_len) {
    double number = 0;
    if (sscanf(*expr, "%lf", &number))
        push(my_stack, number);
    int number_length = get_length_of_number(*expr, len);
    int tmp_mant_length = get_mantissa_length(*expr, number_length);
    if (tmp_mant_length > *mant_len)
        *mant_len = tmp_mant_length;
    *expr += number_length;
}

void process_operator(my_stack_t **my_stack, char **expr) {
    double num1, num2;
    pop(my_stack, &num1);
    pop(my_stack, &num2);
    if (**expr == '+')
        push(my_stack, num2 + num1);
    else if (**expr == '-')
        push(my_stack, num2 - num1);
    else if (**expr == '*')
        push(my_stack, num2 * num1);
    else if (**expr == '/')
        push(my_stack, num2 / num1);
    else if (**expr == '%')
        push(my_stack, fmod(num2, num1));
    else if (**expr == '^')
        push(my_stack, pow(num2, num1));
    *expr += 1;
}

void process_function(char **expr, my_stack_t **my_stack, int func_len) {
    double number = 0;
    pop(my_stack, &number);
    if (is_sin(*expr))
        push(my_stack, sin(number));
    else if (is_cos(*expr))
        push(my_stack, cos(number));
    else if (is_tan(*expr))
        push(my_stack, tan(number));
    else if (is_ln(*expr))
        push(my_stack, log(number));
    else if (is_log(*expr))
        push(my_stack, log10(number));
    else if (is_acos(*expr))
        push(my_stack, acos(number));
    else if (is_asin(*expr))
        push(my_stack, asin(number));
    else if (is_sqrt(*expr))
        push(my_stack, sqrt(number));
    else if (is_atan(*expr))
        push(my_stack, atan(number));
    *expr += func_len;
}
