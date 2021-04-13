#ifndef VM_X2017_H
#define VM_X2017_H

#include "parser.h"
#include "start.h"

#include <err.h>

// 0 is the entry point
#define MAIN_FUNC 0

// we have 2^8 bytes of ram, and 8 1 byte registers
#define RAM_SIZE (1 << 8)
#define NUM_REGISTERS 8

// special registers
#define STACK_PTR (registers[6])
#define PROG_CTR (registers[7])

// use first chunk of ram for function addresses and frame sizes
#define INSTR_ADDR(FUNC_LABEL) (ram[FUNC_LABEL])
#define FRAME_SIZE(FUNC_LABEL) (ram[MAX_FUNCTIONS + FUNC_LABEL])

// since we used the first chunk of the ram already, the stack doesn't start
// until a particular point
#define STACK_START (MAX_FUNCTIONS * 2)
#define STACK_MAX UINT8_MAX
// helper to locate a particular stack symbol within the frame
#define STACK_LOC(X) (STACK_PTR - (X))

void vm_x2017(func_t* functions);

uint8_t run_instruction(const inst_t inst, uint8_t* ram, uint8_t* registers);
void mov(const arg_t arg, uint8_t* ram, uint8_t* registers);
uint8_t arg_value(const arg_t arg, const uint8_t* ram, uint8_t* registers);
void call_function(uint8_t label, uint8_t* ram, uint8_t* registers);

#endif
