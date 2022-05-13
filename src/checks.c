#include "checks.h"

int is_unary(char ch) {
    return ch == '+' || ch == '-';
}

int is_x(char ch) {
    return ch == 'x';
}


int check_for_x(const char *expr) {
    int result = 0;
    int len = strlen(expr);
    for (int i = 0; i < len; i++) {
        if (is_x(expr[i])) {
           result = 1;
        }
    }
    return result;
}

static int is_any_other_error(const char *expr) {
    int error = 0;
    int len = strlen(expr) - 1;
    while (is_close_bracket(expr[len])) {
        len--;
    }
    if (is_binary_operator(expr[len]))
        error = 1;

    int has_number = 0;
    while (is_number(expr[len])) {
        len--;
        has_number = 1;
    }
    if (has_number && is_dot(expr[len])) {
        if (!is_number(expr[len - 1]) || is_open_bracket(expr[len - 1]))
            error = 1;
    }
        return error;
}

static void loop_number(const char *expr, int *index) {
    while (is_number(expr[*index])) {
        *index += 1;
    }
}

static int is_number_error(const char *expr, int len) {
    int error = 0;
    for (int i = 0; i < len; i++) {
        if (is_number(expr[i])) {
            if (is_close_bracket(expr[i - 1]) ||
            (expr[i + 1] >= 'a' && expr[i + 1] <= 'z' && expr[i+1] != 'm'))
                error = 1;
        }
    }
    for (int i = 0; i < len; i++) {
        if (is_number(expr[i])) {
            loop_number(expr, &i);
            if (is_dot(expr[i++])) {
                loop_number(expr, &i);
                if (is_dot(expr[i]) || (expr[i] >= 'a' && expr[i] <= 'z' && expr[i] != 'm')) {
                    error = 1;
                }
            }
        }
    }
    return error;
}

int is_function_key(unsigned int keyval) {
    return keyval == 's' || keyval == 'c' || keyval == 't'
        || keyval == 'S' || keyval == 'C' || keyval == 'T'
        || keyval == 'q' || keyval == 'l' || keyval == 'L';
}

int is_invalid_expr(const char *expr) {
    int result = 0;
    int bracket_error = 0;
    int mod_err = 0;
    int expr_len = strlen(expr);
    int any_error =  is_binary_operator(expr[expr_len - 1])
        || is_dot(expr[expr_len - 1])
        || is_any_other_error(expr);

    int number_error = is_number_error(expr, expr_len);
    int x_error = 0;
    int func_err = 0;
    for (int i = 0; i < expr_len; i++) {
        if (expr[i] >= 'a' && expr[i] <= 'z' && expr[i] != 'm') {
            if (is_close_bracket(expr[i-1]) || is_x(expr[i - 1]))
                func_err = 1;
        }
    }
    for (int i = 0; i < expr_len; i++) {
        if (is_x(expr[i])) {
            if (is_number(expr[i-1]) || is_number(expr[i+1]) || is_dot(expr[i - 1]))
                x_error = 1;
        }
    }
    if (expr_len > 3) {
        if (is_mod((char*)&expr[expr_len - 3]))
            mod_err = 1;
    }
    if (!any_error || !mod_err) {
        for (int i = 0; i < expr_len; i++) {
            if (is_open_bracket(expr[i]))
               if (is_close_bracket(expr[i+1]))
                  bracket_error = 1;
        }
    }
    result = mod_err + any_error + bracket_error + number_error + x_error + func_err;
    return result;
}
