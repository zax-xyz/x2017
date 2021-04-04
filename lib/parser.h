#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdio.h>

#define MAX_FUNCTIONS (1 << 3) /* 3 bits for function label */
#define MAX_INSTRUCTIONS (1 << 5) /* 5 bits for number of instructions */

typedef enum { MOV, CAL, RET, REF, ADD, PRINT, NOT, EQU } OPCODE;
typedef enum { VAL, REG, STACK, PTR } FIELD_TYPE;

typedef struct {
    FIELD_TYPE type;
    uint8_t value;
} argument_t;

typedef struct {
    OPCODE opcode;
    argument_t first_arg;
    argument_t second_arg;
} instruction_t;

typedef struct {
    uint8_t label;
    uint8_t size;
    instruction_t instructions[MAX_INSTRUCTIONS];
} function_t;

extern function_t functions[MAX_FUNCTIONS];

void parse(FILE* fp);
argument_t parse_arg(FILE* fp, uint16_t* buffer, uint8_t* buffer_len,
        long* offset);
uint8_t parse_val(FILE* fp, long* offset, uint16_t* buffer, uint8_t* buffer_len,
        uint8_t length);

#endif
