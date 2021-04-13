#ifndef OBJDUMP_X2017_H
#define OBJDUMP_X2017_H

#include "parser.h"
#include "start.h"

#include <err.h>

/*
 * The bulk of the program. Outputs the x2017 assembly instructions in a human-
 * readable format
 */
void objdump(const func_t* functions);

/*
 * Prints a single argument from an x2017 instruction in a human-readable format
 */
void print_arg(const arg_t arg, const char** field_types);

#endif
