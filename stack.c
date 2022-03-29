#include "stack.h"

my_stack_t* init_my_stack(const my_stack_type_t type) {
    my_stack_t *my_stack = (my_stack_t*) malloc(sizeof(my_stack_t));
    if (type == STACK_CHAR)
        my_stack->value.data_val.char_value = 0;
    else
        my_stack->value.data_val.double_value = 0.0;
    my_stack->value_type = type;
    my_stack->next = NULL;
    return my_stack;
}


void pop(my_stack_t **my_stack, void *pointer) {
    my_stack_t *s = *my_stack;
    if (s->value_type == STACK_CHAR)
        *((char*)pointer) = s->value.data_val.char_value;
    else
        *((double*)pointer) = s->value.data_val.double_value;
    *my_stack = s->next;
    free(s);
}

void push(my_stack_t **my_stack, ...) {
    my_stack_t *s = (my_stack_t*) malloc(sizeof(my_stack_t));
    s->value_type = (*my_stack)->value_type;
    va_list list;
    va_start(list, my_stack);
    if (s->value_type == STACK_CHAR)
        s->value.data_val.char_value = (char)va_arg(list, int);
    else
        s->value.data_val.double_value = va_arg(list, double);
    s->next = *my_stack;
    *my_stack = s;
    va_end(list);
}

void destroy_my_stack(my_stack_t *my_stack) {
    char char_tmp = 0;
    double double_tmp = 0;
    while (!is_empty_my_stack(my_stack)) {
        if (my_stack->value_type == STACK_CHAR)
            pop(&my_stack, &char_tmp);
        else
            pop(&my_stack, &double_tmp);
    }
    free(my_stack);
}

int is_empty_my_stack(my_stack_t *my_stack) {
    return my_stack->next == NULL;
}
