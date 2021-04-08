#include "vm_x2017.h"

#include <err.h>

#define STACK_START (MAX_FUNCTIONS * 2)
#define STACK_MAX UINT8_MAX
#define STACK_LOC(X) (registers[6] - (X))

int main(int argc, char** argv) {
    if (argc != 2)
        errx(1, "Incorrect number of arguments. Please provide a single binary "
                "file path.");

    func_t functions[MAX_FUNCTIONS] = {{ .size = 0 }};

    parse(argv[1], functions);
    vm_x2017(functions);

    return 0;
}

void vm_x2017(func_t* functions) {
    uint8_t ram[RAM_SIZE];

    // initialise function addresses. we have a default value in order to check
    // if a main function is present. note that this default value is impossible
    // to be a function entry point as it can only be used for the last
    // instruction, with every function having maximum number of instructions
    for (int i = 0; i < MAX_FUNCTIONS; i++)
	ram[i] = MAX_INSTRUCTIONS;

    uint8_t registers[NUM_REGISTERS];

    inst_t instructions[MAX_INSTRUCTIONS_TOTAL];
    uint8_t instr_idx = 0;

    for (uint8_t i = 0; i < MAX_FUNCTIONS; i++) {
        func_t func = functions[MAX_FUNCTIONS - i - 1];

	if (!func.size)
	    continue;

	// use first part of ram for the function instruction addresses
	ram[func.label] = instr_idx;

	for (uint8_t j = 0; j < func.size; j++, instr_idx++) {
	    instructions[instr_idx] = func.instructions[j];
	}

	// next chunck of ram is used for the stack frame sizes
	ram[MAX_FUNCTIONS + func.label] = func.frame_size;
    }

    if (ram[0] == UINT8_MAX)
	errx(1, "No main function found");

    registers[7] = ram[0];

    // frame pointer
    // we'll have our stack frames backwards to make it easier to add them
    registers[6] = STACK_START + ram[MAX_FUNCTIONS];

    while (!run_instruction(instructions[registers[7]++], ram, registers)) {}

    return;
}

uint8_t run_instruction(const inst_t inst, uint8_t* ram, uint8_t* registers) {
    switch (inst.opcode) {
    case MOV:
	// copy value from B to A
	registers[4] = arg_value(inst.arg2, ram, registers);
	mov(inst.arg1, ram, registers);

	break;
    case CAL:
	// call another function
	call_function(inst.arg1.value, ram, registers);
	break;
    case RET:
	registers[4] = STACK_START + ram[MAX_FUNCTIONS];
	if (registers[6] == registers[4])
	    return 1;

	registers[5] = registers[6] + 1;
	registers[6] = ram[registers[5]];
	registers[5]++;
	registers[7] = ram[registers[5]];
	break;
    case REF:
	// store the address of stack symbol B into A
	registers[4] = STACK_LOC(inst.arg2.value);
	mov(inst.arg1, ram, registers);
	break;
    case ADD:
	// add registers A and B, storing into A
	registers[inst.arg1.value] += registers[inst.arg2.value];
	break;
    case PRINT:
	// print value at address as uint
	registers[4] = arg_value(inst.arg1, ram, registers);
	printf("%u\n", registers[4]);
	break;
    case NOT:
	// inplace bitwise not on register
	registers[inst.arg1.value] = ~registers[inst.arg1.value];
	break;
    case EQU:
	// tests if a register is equal to 0
	registers[inst.arg1.value] = registers[inst.arg1.value] == 0;
	break;
    }

    return 0;
}

void mov(const arg_t arg, uint8_t* ram, uint8_t* registers) {
    switch (arg.type) {
    case REG:
	registers[arg.value] = registers[4];
	break;
    case STACK:
	registers[5] = STACK_LOC(arg.value);
	ram[registers[5]] = registers[4];
	break;
    case PTR:
	registers[5] = STACK_LOC(arg.value);
	registers[5] = ram[registers[5]];
	ram[registers[5]] = registers[4];
	break;
    case VAL:
	// our parser already ensures it can't be VAL
	break;
    }
}

uint8_t arg_value(const arg_t arg, const uint8_t* ram, uint8_t* registers) {
    switch (arg.type) {
    case VAL:
	return arg.value;
    case REG:
	return registers[arg.value];
    case STACK:
	registers[4] = STACK_LOC(arg.value);
	return ram[registers[4]];
    case PTR:
	registers[4] = STACK_LOC(arg.value);
	registers[5] = ram[registers[4]];
	return ram[registers[5]];
    default:
	// unreached
	return 0;
    }
}

void call_function(uint8_t label, uint8_t* ram, uint8_t* registers) {
    registers[4] = STACK_MAX - ram[MAX_FUNCTIONS + label];
    if (registers[6] > registers[4])
	errx(1, "Stack overflow detected when trying to call function %d",
		label);

    // new frame pointer
    registers[4] = registers[6] + 2 + ram[MAX_FUNCTIONS + label];

    // store previous frame pointer
    registers[5] = registers[4] + 1;
    ram[registers[5]] = registers[6];

    // store previous program counter
    registers[5]++;
    ram[registers[5]] = registers[7];

    // update frame pointer and program counter
    registers[6] = registers[4];
    registers[7] = ram[label];
}
