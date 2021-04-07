#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdio.h>

#define MAX_FUNCTIONS (1 << 3) // 3 bits for function label
#define MAX_INSTRUCTIONS (1 << 5) // 5 bits for number of instructions
#define MAX_INSTRUCTIONS_TOTAL (MAX_FUNCTIONS * MAX_INSTRUCTIONS)

#define FUNC_SIZE 3
#define OPCODE_SIZE 3
#define ARG_TYPE_SIZE 2
#define VALUE_SIZE 8
#define REG_SIZE 3
#define STACK_SIZE 5
#define PTR_SIZE 5
#define INSTR_SIZE 5

typedef struct {
    uint16_t data;
    uint8_t size;
} buf_t;

typedef enum { MOV, CAL, RET, REF, ADD, PRINT, NOT, EQU } OPCODE;
typedef enum { VAL, REG, STACK, PTR } FIELD_TYPE;

typedef struct {
    FIELD_TYPE type;
    uint8_t value;
} arg_t;

typedef struct {
    OPCODE opcode;
    arg_t arg1;
    arg_t arg2;
} inst_t;

typedef struct {
    uint8_t label;
    uint8_t size;
    inst_t instructions[MAX_INSTRUCTIONS];
    uint8_t frame_size;
} func_t;

void parse(const char* filename, func_t* functions);
void parse_inst(inst_t* inst, FILE* fp, buf_t* buffer, long* offset);
arg_t parse_arg(FILE* fp, buf_t* buffer, long* offset);
uint8_t parse_val(FILE* fp, long* offset, buf_t* buffer, const uint8_t length);

void map_stack(inst_t* inst, uint8_t* stack_symbols, uint8_t* stack_idx);
void map_symbol(uint8_t* val, uint8_t* stack_symbols, uint8_t* stack_idx);

#endif
