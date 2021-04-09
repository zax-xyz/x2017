#include "objdump_x2017.h"

#include <err.h>

// _start function specific to x86-64 linux
asm(
    ".global _start\n\t"
    "_start:"
    "    movq   (%rsp), %rdi\n\t" // argc is in rsp (register stack pointer)
    "    leaq   8(%rsp), %rsi\n\t" // argv pointer
    "    call   main\n\t"

    "    movl   %eax, %edi\n\t" // pass main's return value to exit
    "    call   exit\n\t"
);

int main(int argc, char** argv) {
    if (argc != 2)
        errx(1, "Incorrect number of arguments. Please provide a single binary "
                "file path.");

    func_t functions[MAX_FUNCTIONS] = {{ .size = 0 }};

    parse(argv[1], functions);
    objdump(functions);

    return 1;
}

void objdump(const func_t* functions) {
    const char* opcodes[] = {
        "MOV", "CAL", "RET", "REF", "ADD", "PRINT", "NOT", "EQU"
    };
    const char* field_types[] = {"VAL", "REG", "STK", "PTR"};

    for (uint8_t i = 0; i < MAX_FUNCTIONS; i++) {
        func_t func = functions[MAX_FUNCTIONS - i - 1];
        if (!func.size)
            continue;

        printf("FUNC LABEL %d\n", func.label);

        for (uint8_t j = 0; j < func.size; j++) {
            const OPCODE opcode = func.instructions[j].opcode;
            printf("    %s", opcodes[opcode]);

            if (opcode != RET)
                print_arg(func.instructions[j].arg1, field_types);

            if (opcode == MOV || opcode == REF || opcode == ADD)
                print_arg(func.instructions[j].arg2, field_types);

            printf("\n");
        }
    }
}

void print_arg(const arg_t arg, const char** field_types) {
    if (arg.type == STACK || arg.type == PTR) {
        if (arg.value >= 26) {
            printf(" %s %c", field_types[arg.type], arg.value - 26 + 'a');
        } else {
            printf(" %s %c", field_types[arg.type], arg.value + 'A');
        }
    } else {
        printf(" %s %d", field_types[arg.type], arg.value);
    }
}
