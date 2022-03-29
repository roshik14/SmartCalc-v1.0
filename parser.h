#ifndef PARSER_H_
#define PARSER_H_
#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SIN  's'
#define COS  'c'
#define TAN  't'
#define ACOS 'C'
#define ASIN 'S'
#define ATAN 'T'
#define SQRT 'q'
#define LN   'L'
#define LOG  'l'

#define MAX_LEN 255*2
#define FLOAT_LEN 50
#define FUNC_MAX 10

char* convert_expr(char*);
char* get_function_name(char);
char* get_expr(char*);
int parse_expr(char**, char**, my_stack_t**);
int is_binary_operator(char);
int is_unary_operator(char**, char);
int get_operator_priority(char);
int get_length_of_number(char*, int);
int get_mantissa_length(char*, int);
int get_function_code(char*);
int is_number(char);
int is_open_bracket(char);
int is_close_bracket(char);
int is_mod(char*);
int is_function_in_expr(char*);
int is_function_in_my_stack(char);
int is_ln(char*);
int is_sin(char*);
int is_cos(char*);
int is_tan(char*);
int is_log(char*);
int is_sqrt(char*);
int is_acos(char*);
int is_asin(char*);
int is_atan(char*);
int is_float(char*);
int is_dot(char);
int check_operators_priorities(char, char);
int check_my_stack_for_brackets(my_stack_t *my_stack);
void write_num_to_output(char**, char**);
void write_int_to_output(char**, char**);
void write_float_to_output(char**, char**);
void write_operator_to_output(char**, my_stack_t**);
void read_binary_operator(char**, char**, my_stack_t**);
void read_function(char**, int, my_stack_t**);
void read_open_bracket(char**, my_stack_t**);
void read_close_bracket(char**, char**, my_stack_t**, int*);
#endif  // PARSER_H_
