#ifndef OBJDUMP_X2017_H
#define OBJDUMP_X2017_H

#include "parser.h"

void objdump(function_t* functions, uint8_t size);
void print_arg(argument_t arg, const char** field_types);

#endif
