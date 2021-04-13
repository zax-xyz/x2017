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

/*
 * The bulk of the program. Executes the x2017 program.
 */
void vm_x2017(func_t* functions);

/*
 * Executes a single vm instruction
 */
uint8_t run_instruction(const inst_t inst, uint8_t* ram, uint8_t* registers);

/*
 * Helper to copy a value from one place to another in the vm
 */
void mov(const arg_t arg, uint8_t* ram, uint8_t* registers);

/*
 * Gets the value from an argument
 *
 * For a VAL argument type, simply returns the raw value. For a REG argument,
 * returns the value stored in the corresponding register. For a STACK argument,
 * returns the value stored in the stack for that stack symbol. For a PTR
 * argument, treats it as a stack symbol, reads that symbol's value, then using
 * that value as a memory address, returns the value stored at that address.
 */
uint8_t arg_value(const arg_t arg, const uint8_t* ram, uint8_t* registers);

/*
 * Calls an x2017 function by its label
 */
void call_function(uint8_t label, uint8_t* ram, uint8_t* registers);

#endif
