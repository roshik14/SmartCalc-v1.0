#include "parser.h"

char* convert_expr(char *expr) {
    char *res;
    if (!strlen(expr))
        res = NULL;
    else
       res = get_expr(expr);
    return res;
}

char* get_expr(char *expr) {
    char *res = (char*) calloc(MAX_LEN, sizeof(char));
    my_stack_t *my_stack = init_my_stack(STACK_CHAR);
    int error = parse_expr(&expr, &res, &my_stack);
    while (!is_empty_my_stack(my_stack) && !error) {
        if (is_open_bracket(my_stack->value.data_val.char_value))
            error = 1;
        else
            write_operator_to_output(&res, &my_stack);
    }
    if (error) {
        free(res);
        res = NULL;
    }
    destroy_my_stack(my_stack);
    return res;
}

int parse_expr(char **expr, char **res, my_stack_t **my_stack) {
    int func_len;
    char first = **expr;
    int error = 0;
    while (**expr) {
        if (error == 0) {
            if (is_number(**expr)) {
                write_num_to_output(expr, res);
            } else if (is_unary_operator(expr, first)) {
                char *null_str = "0";
                strncat(*res, null_str, 2);
                read_binary_operator(expr, res, my_stack);
            } else if ((func_len = is_function_in_expr(*expr))) {
                read_function(expr, func_len, my_stack);
            } else if (is_binary_operator(**expr) || is_mod(*expr)) {
                read_binary_operator(expr, res, my_stack);
            } else if (is_open_bracket(**expr)) {
                read_open_bracket(expr, my_stack);
            } else if (is_close_bracket(**expr)) {
                read_close_bracket(expr, res, my_stack, &error);
            }
        } else {
            *expr += 1;
        }
    }
    return error;
}
void read_binary_operator(char **expr, char **res, my_stack_t **my_stack) {
    char current = is_mod(*expr) ? '%' : **expr;
    while (check_operators_priorities(current, (*my_stack)->value.data_val.char_value)) {
        write_operator_to_output(res, my_stack);
    }
    push(my_stack, current);
    char *space = " ";
    strncat(*res, space, 2);
    if (is_mod(*expr))
        *expr += 3;
    else
        *expr += 1;
}

void read_function(char **expr, int func_len, my_stack_t **my_stack) {
    push(my_stack, get_function_code(*expr));
    *expr += func_len;
}

void read_open_bracket(char **expr, my_stack_t **my_stack) {
    push(my_stack, **expr);
    *expr += 1;
}

void read_close_bracket(char **expr, char **res, my_stack_t **my_stack, int *error) {
    if (check_my_stack_for_brackets(*my_stack)) {
        while (!is_open_bracket((*my_stack)->value.data_val.char_value) && *error == 0) {
            char current = (*my_stack)->value.data_val.char_value;
            if (current == 0 || is_close_bracket(current))
                *error = 1;
            else
                write_operator_to_output(res, my_stack);
        }
    } else {
        *error = 1;
    }
    if (*error == 0) {
        char tmp = 0;
        pop(my_stack, &tmp);
        if (is_function_in_my_stack((*my_stack)->value.data_val.char_value)) {
            char f;
            pop(my_stack, &f);
            char *func_name = get_function_name(f);
            strncat(*res, func_name, 6);
            free(func_name);
        }
        *expr += 1;
    }
}

void write_num_to_output(char **expr, char **res) {
    double num = 0;
    if (sscanf(*expr, "%lf", &num)) {
        char *tmp = calloc(FLOAT_LEN, sizeof(char));
        int expr_len = strlen(*expr);
        int number_length = get_length_of_number(*expr, expr_len);
        int mant_length = get_mantissa_length(*expr, number_length);
        snprintf(tmp, FLOAT_LEN, "%.*lf", mant_length, num);
        int tmp_len = strlen(tmp);
        strncat(*res, tmp, tmp_len);
        *expr += number_length;
        free(tmp);
    }
}

int is_unary_operator(char **expr, char first) {
    int result = 0;
    if (**expr == '-' || **expr == '+') {
        if (**expr == first || (!is_number(*(*expr - 1)) && !is_number(*(*expr - 2)))) {
            result = 1;
        }
    }
    return result;
}

int get_mantissa_length(char *expr, int len) {
    int dot = 0;
    int res = 0;
    for (int i = 0; i < len; i++) {
        if (is_dot(expr[i])) {
            dot = i + 1;
            i = len;
        }
    }
    if (dot != 0) {
        while (dot < len && is_number(expr[dot])) {
            res++;
            dot++;
        }
    }
    return res;
}


int check_my_stack_for_brackets(my_stack_t *my_stack) {
    int res = 0;
    my_stack_t *s = my_stack;
    do {
     if (is_open_bracket(s->value.data_val.char_value))
         res = 1;
     s = s->next;
    } while (s->next != NULL);
    return res;
}


int get_length_of_number(char *expr, int len) {
    int res = 0;
    int i = 0;
    while (i < len && (is_number(expr[i]) || expr[i] == '.')) {
        res++;
        i++;
    }
    return res;
}

int get_function_code(char *expr) {
    int res = 0;
    if (is_sin(expr))
        res = SIN;
    if (is_cos(expr))
        res = COS;
    if (is_tan(expr))
        res = TAN;
    if (is_ln(expr))
        res = LN;
    if (is_log(expr))
        res = LOG;
    if (is_sqrt(expr))
        res = SQRT;
    if (is_acos(expr))
        res = ACOS;
    if (is_asin(expr))
        res = ASIN;
    if (is_atan(expr))
        res = ATAN;
    return res;
}

char* get_function_name(char ch) {
    char *name = (char*)malloc(FUNC_MAX * sizeof(char));
    if (ch == SIN)
        snprintf(name, FUNC_MAX, "%s", "sin");
    else if (ch == COS)
        snprintf(name, FUNC_MAX, "%s", "cos");
    else if (ch == TAN)
        snprintf(name, FUNC_MAX, "%s", "tan");
    else if (ch == SQRT)
        snprintf(name, FUNC_MAX, "%s", "sqrt");
    else if (ch == LOG)
        snprintf(name, FUNC_MAX, "%s", "log");
    else if (ch == LN)
        snprintf(name, FUNC_MAX, "%s", "ln");
    else if (ch == ACOS)
        snprintf(name, FUNC_MAX, "%s", "acos");
    else if (ch == ASIN)
        snprintf(name, FUNC_MAX, "%s", "asin");
    else if (ch == ATAN)
        snprintf(name, FUNC_MAX, "%s", "atan");
    return name;
}

void write_operator_to_output(char **res, my_stack_t **my_stack) {
    char oper_in_my_stack;
    pop(my_stack, &oper_in_my_stack);
    char oper_str[3];
    snprintf(oper_str, sizeof(oper_str), " %c", oper_in_my_stack);
    strncat(*res, oper_str, 4);
}

int check_operators_priorities(char oper1, char oper2) {
    int result = 0;
    int oper1_prior = get_operator_priority(oper1);
    int oper2_prior = get_operator_priority(oper2);
    if (oper1_prior <= oper2_prior)
        result = 1;
    return result;
}

int get_operator_priority(char operator) {
    int priority = 0;
    if (operator == '*' || operator == '/' || operator == '%')
        priority = 2;
    else if (operator == '+' || operator == '-')
        priority = 1;
    else if (operator == '^')
        priority = 3;
    return priority;
}

int is_binary_operator(char ch) {
    return ch == '-' || ch == '*' || ch == '+' || ch == '/' || ch == '^';
}

int is_mod(char *str) {
    return !strncmp(str, "mod", 3);
}

int is_function_in_my_stack(char ch) {
    return ch == SIN || ch == COS || ch == TAN
        || ch == LOG || ch == LN || ch == SQRT
        || ch == ATAN || ch == ACOS || ch == ASIN;
}

int is_function_in_expr(char *str) {
    int result = 0;
    if (is_ln(str))
        result = 2;
    else if (is_sin(str) || is_cos(str) || is_tan(str) || is_log(str))
        result = 3;
    else if (is_asin(str) || is_acos(str) || is_atan(str) || is_sqrt(str))
        result = 4;
    return result;
}

int is_ln(char *str) {
    return !strncmp(str, "ln", 2);
}

int is_sin(char *str) {
    return !strncmp(str, "sin", 3);
}

int is_cos(char *str) {
    return !strncmp(str, "cos", 3);
}

int is_tan(char *str) {
    return !strncmp(str, "tan", 3);
}

int is_log(char *str) {
    return !strncmp(str, "log", 3);
}

int is_acos(char *str) {
    return !strncmp(str, "acos", 4);
}

int is_asin(char *str) {
    return !strncmp(str, "asin", 4);
}

int is_sqrt(char *str) {
    return !strncmp(str, "sqrt", 4);
}

int is_atan(char *str) {
    return !strncmp(str, "atan", 4);
}

int is_open_bracket(char ch) {
    return ch == '(';
}

int is_close_bracket(char ch) {
    return ch == ')';
}

int is_number(char ch) {
    return ch >= '0' && ch <= '9';
}

int is_dot(char ch) {
    return ch == '.';
}
