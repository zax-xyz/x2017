#include "objdump_x2017.h"

#include <err.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 2)
        errx(1, "Incorrect number of arguments. Please provide a single binary "
                "file path.");

    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL)
        errx(1, "Error reading file");

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size < 1)
        errx(1, "Invalid binary");

    long offset = size;
    uint16_t buffer = 0;
    uint8_t buffer_len = 0;

    function_t functions[MAX_FUNCTIONS] = {{ .size = 0 }};
    uint8_t function_idx = 0;

    while (offset > 1) {
        uint8_t instructions = parse_val(fp, &offset, &buffer, &buffer_len, 5);
        if (!instructions)
            errx(1, "function must have at least 1 instruction (RET).");

        function_t function;
        function.size = instructions;

        uint8_t opcode = parse_val(fp, &offset, &buffer, &buffer_len, 3);
        if (opcode != RET)
            errx(1, "no RET instruction found at end of function.");

        function.instructions[instructions - 1].opcode = RET;

        for (uint8_t i = 2; i <= instructions; i++) {
            uint8_t opcode = parse_val(fp, &offset, &buffer, &buffer_len, 3);
            instruction_t inst = { opcode };
            switch (opcode) {
                case MOV:
                    inst.first_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.first_arg.type == VAL)
                        errx(1, "first argument to MOV cannot be value typed.");

                    inst.second_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    break;
                case CAL:
                    inst.first_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.first_arg.type != VAL)
                        errx(1, "first argument to CAL must be value typed.");
                    break;
                case REF:
                    inst.first_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.first_arg.type == VAL)
                        errx(1, "first argument to REF cannot be value typed.");

                    inst.second_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.second_arg.type != STACK)
                        errx(1, "second argument to REF must be stack typed.");

                    break;
                case ADD:
                    inst.first_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.first_arg.type != REG)
                        errx(1, "first argument to ADD must be register typed.");

                    inst.second_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.second_arg.type != REG)
                        errx(1, "second argument to ADD must be register typed.");
                    break;
                case PRINT:
                    inst.first_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.first_arg.type == VAL)
                        errx(1, "first argument to ADD must not be value typed.");

                    break;
                case NOT:
                    inst.first_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.first_arg.type != REG)
                        errx(1, "first argument to NOT must be register typed.");

                    break;
                case EQU:
                    inst.first_arg = parse_arg(fp, &buffer, &buffer_len, &offset);
                    if (inst.first_arg.type != REG)
                        errx(1, "first argument to EQU must be register typed.");

                    break;
            }

            function.instructions[instructions - i] = inst;
        }

        function.label = parse_val(fp, &offset, &buffer, &buffer_len, 3);

        for (int i = 0; i < function_idx; i++) {
            if (function.label == functions[i].label) {
                errx(1, "function already defined with label %d",
                        function.label);
            }
        }

        functions[function_idx++] = function;
    }

    fclose(fp);

    for (int i = 0; i < MAX_FUNCTIONS; i++) {
        function_t func = functions[MAX_FUNCTIONS - i - 1];
        if (!func.size)
            continue;

        printf("FUNC LABEL %d\n", func.label);

        for (int j = 0; j < func.size; j++) {
            uint8_t opcode = func.instructions[j].opcode;
            printf("    %s", opcodes[opcode]);

            if (opcode != RET) {
                argument_t arg = func.instructions[j].first_arg;

                if (arg.type == STACK || arg.type == PTR) {
                    printf(" %s %c", field_types[arg.type], arg.value + 'A');
                } else {
                    printf(" %s %d", field_types[arg.type], arg.value);
                }
            }

            argument_t arg = func.instructions[j].second_arg;
            if (opcode == MOV || opcode == REF || opcode == ADD) {
                if (arg.type == STACK) {
                    printf(" %s %c", field_types[arg.type], arg.value + 'A');
                } else {
                    printf(" %s %d", field_types[arg.type], arg.value);
                }
            }

            printf("\n");
        }
    }

    return 0;
}

argument_t parse_arg(FILE* fp, uint16_t* buffer, uint8_t* buffer_len,
        long* offset) {
    argument_t arg;

    arg.type = parse_val(fp, offset, buffer, buffer_len, 2);

    switch (arg.type) {
        case VAL:
            arg.value = parse_val(fp, offset, buffer, buffer_len, 8);
            break;
        case REG:
            arg.value = parse_val(fp, offset, buffer, buffer_len, 3);
            break;
        case STACK:
        case PTR:
            arg.value = parse_val(fp, offset, buffer, buffer_len, 5);
            break;
    }

    return arg;
}

uint8_t parse_val(FILE* fp, long* offset, uint16_t* buffer, uint8_t* buffer_len,
        uint8_t length) {
    if (*buffer_len < length)
        extend_buffer(fp, buffer, buffer_len, offset);

    uint8_t value = *buffer & ((1 << length) - 1);
    *buffer >>= length;
    *buffer_len -= length;
    return value;
}

void extend_buffer(FILE* fp, uint16_t* buffer, uint8_t* buffer_len,
        long* offset) {
    *offset -= 1;
    if (*offset < 0)
        errx(1, "reached unexpected beginning of file while trying to parse.");

    fseek(fp, *offset, SEEK_SET);
    uint8_t buf;
    fread(&buf, 1, 1, fp);

    *buffer += buf << *buffer_len;
    *buffer_len += 8;
}
