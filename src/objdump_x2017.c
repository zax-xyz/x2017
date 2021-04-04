#include "objdump_x2017.h"

#include <err.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 2)
        errx(1, "Incorrect number of arguments. Please provide a single binary "
                "file path.");

    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL)
        errx(1, "Error opening file");

    function_t* functions = parse(fp);
    objdump(functions, MAX_FUNCTIONS);

    return 0;
}

void objdump(function_t* functions, uint8_t size) {
    const char* opcodes[] = {
        "MOV", "CAL", "RET", "REF", "ADD", "PRINT", "NOT", "EQU"
    };
    const char* field_types[] = {"VAL", "REG", "STK", "PTR"};

    for (uint8_t i = 0; i < size; i++) {
        function_t func = functions[size - i - 1];
        if (!func.size)
            continue;

        printf("FUNC LABEL %d\n", func.label);

        for (uint8_t j = 0; j < func.size; j++) {
            const OPCODE opcode = func.instructions[j].opcode;
            printf("    %s", opcodes[opcode]);

            if (opcode != RET)
                print_arg(func.instructions[j].first_arg, field_types);

            if (opcode == MOV || opcode == REF || opcode == ADD)
                print_arg(func.instructions[j].second_arg, field_types);

            printf("\n");
        }
    }
}

void print_arg(argument_t arg, const char** field_types) {
    if (arg.type == STACK || arg.type == PTR) {
        printf(" %s %c", field_types[arg.type], arg.value + 'A');
    } else {
        printf(" %s %d", field_types[arg.type], arg.value);
    }
}
