#ifndef STACK_H_
#define STACK_H_
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

typedef enum {
    STACK_CHAR,
    STACK_DOUBLE
} my_stack_type_t;

typedef struct my_stack_value_struct {
    union {
        char char_value;
        double double_value;
    } data_val;
} my_stack_value_t;


typedef struct my_stack_struct {
    struct my_stack_value_struct value;
    struct my_stack_struct *next;
    my_stack_type_t value_type;
} my_stack_t;

my_stack_t* init_my_stack(const my_stack_type_t type);
void pop(my_stack_t**, void*);
int is_empty_my_stack(my_stack_t*);
void push(my_stack_t**, ...);
void destroy_my_stack(my_stack_t*);

#endif  // STACK_H_
