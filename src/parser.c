#include "parser.h"

#include <err.h>
#include <stdlib.h>

function_t functions[MAX_FUNCTIONS] = {{ .size = 0 }};

void parse(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long offset = ftell(fp);
    if (offset < 1)
        errx(1, "Invalid binary");

    uint16_t buffer = 0;
    uint8_t buffer_len = 0;

    uint8_t function_idx = 0;

    while (offset > 1) {
        uint8_t instructions = parse_val(fp, &offset, &buffer, &buffer_len, 5);
        if (!instructions)
            errx(1, "function must have at least 1 instruction (RET).");

        function_t function = { .size = instructions };

        const uint8_t opcode = parse_val(fp, &offset, &buffer, &buffer_len, 3);
        if (opcode != RET)
            errx(1, "no RET instruction found at end of function.");

        function.instructions[instructions - 1].opcode = RET;

        for (uint8_t i = 2; i <= instructions; i++) {
            instruction_t inst = {
                .opcode = parse_val(fp, &offset, &buffer, &buffer_len, 3)
            };

            switch (inst.opcode) {
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
                case RET:
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
                errx(1, "multiple function definitions found with label %d",
                        function.label);
            }
        }

        functions[function_idx++] = function;
    }

    fclose(fp);
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
    if (*buffer_len < length) {
        // extend buffer
        *offset -= 1;
        if (*offset < 0)
            errx(1, "reached unexpected beginning of file while trying to parse.");

        fseek(fp, *offset, SEEK_SET);
        uint8_t buf;
        fread(&buf, 1, 1, fp);

        *buffer += buf << *buffer_len;
        *buffer_len += 8;
    }

    const uint8_t value = *buffer & ((1 << length) - 1);
    *buffer >>= length;
    *buffer_len -= length;
    return value;
}
