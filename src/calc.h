#ifndef CALC_H_
#define CALC_H_

#include "parser.h"

#define NUM_MAX_LENGTH 100

char* calculate(char*);
char* make_calculation(char*);
void process_number(char**, my_stack_t**, int, int*);
void process_operator(my_stack_t**, char**);
void process_function(char**, my_stack_t**, int);
#endif  // CALC_H_
