#ifndef OBJDUMP_X2017_H
#define OBJDUMP_X2017_H

#include <stdint.h>
#include <stdio.h>

#define MAX_FUNCTIONS 1 << 3 /* 3 bits for function label */
#define MAX_INSTRUCTIONS 1 << 5 /* 5 bits for number of instructions */

typedef enum { MOV, CAL, RET, REF, ADD, PRINT, NOT, EQU } OPCODE;
typedef enum { VAL, REG, STACK, PTR } FIELD_TYPE;

const char* opcodes[] = {
    "MOV", "CAL", "RET", "REF", "ADD", "ADD", "PRINT","NOT", "EQU"
};
const char* field_types[] = {"VAL", "REG", "STACK", "PTR"};

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
    int8_t size;
    instruction_t instructions[MAX_INSTRUCTIONS];
} function_t;

argument_t parse_arg(FILE* fp, uint16_t* buffer, uint8_t* buffer_len,
        long* offset);
uint8_t parse_val(FILE* fp, long* offset, uint16_t* buffer, uint8_t* buffer_len,
        uint8_t length);
void extend_buffer(FILE* fp, uint16_t* buffer, uint8_t* buffer_len,
        long* offset);

#endif
