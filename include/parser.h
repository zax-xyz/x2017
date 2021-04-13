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

/*
 * A "buffer" for reading from a binary file
 *
 * data stores the data currently read in from the file
 * size stores the number of bits in the buffer
 */
typedef struct {
    uint16_t data;
    uint8_t size;
} buf_t;

typedef enum { MOV, CAL, RET, REF, ADD, PRINT, NOT, EQU } OPCODE;
typedef enum { VAL, REG, STACK, PTR } FIELD_TYPE;

/*
 * A struct to hold an instruction argument
 *
 * type corresponds to the type of field, e.g. value or register
 * value holds the value of the argument
 */
typedef struct {
    FIELD_TYPE type;
    uint8_t value;
} arg_t;

/*
 * A struct to hold an instruction and its information
 *
 * opcode corresponds to the operation code for the instruction, e.d. MOV, CAL
 * arg1 holds the information for the first argument
 * arg2 holds the information for the second argument
 */
typedef struct {
    OPCODE opcode;
    arg_t arg1;
    arg_t arg2;
} inst_t;

/*
 * A struct to hold a function and its data
 *
 * label is the unique function 'label'
 * size is the number of instructions in the function
 * frame_size stores the required stack frame size for the function to hold all
 *      the symbols. It does not include the return address or frame pointer.
 */
typedef struct {
    uint8_t label;
    uint8_t size;
    inst_t instructions[MAX_INSTRUCTIONS];
    uint8_t frame_size;
} func_t;

/*
 * Parses an x2017 binary given its filename, and stores information about its
 * program code into an array of functions
 */
void parse(const char* filename, func_t* functions);

/*
 * Parses an instruction from an x2017 binary file and stores its relevant
 * instruction.
 */
void parse_inst(inst_t* inst, FILE* fp, buf_t* buffer, long* offset);

/*
 * Parses an instruction argument from an x2017 binary file and returns its
 * information.
 */
arg_t parse_arg(FILE* fp, buf_t* buffer, long* offset);

/*
 * Reads a value from an x2017 binary file given a number of bits and returns
 * it.
 */
uint8_t parse_val(FILE* fp, long* offset, buf_t* buffer, const uint8_t length);

/*
 * Maps stack symbols within a function to offsets within the frame. Starts from
 * 0 and increases the offset for each unique stack symbol encountered.
 */
void map_stack(inst_t* inst, uint8_t* stack_symbols, uint8_t* stack_idx);

/*
 * Maps a specific stack symbol to an offset within the stack frame. If the
 * symbol has been encountered before, re-uses the same value, else increases
 * the index for a unique offset.
 */
void map_symbol(arg_t* arg, uint8_t* stack_symbols, uint8_t* stack_idx);

#endif
