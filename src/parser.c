#include "parser.h"

#include <err.h>

void parse(const char* filename, func_t* functions) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
        errx(1, "Error opening file");

    fseek(fp, 0, SEEK_END);
    long offset = ftell(fp);
    if (offset < 1)
        errx(1, "Invalid binary");

    buf_t buffer = {0, 0};

    uint8_t function_idx = 0;

    while (offset > 1) {
        uint8_t instructions = parse_val(fp, &offset, &buffer, INSTR_SIZE);
        func_t function = { .size = instructions };

        // used to map stack symbols to offsets within the stack frames.
        // we could use the compiled value but this would require making the
        // stack frame large for every function to accommodate every potential
        // value. instead we optimise the stack frames to their minimum size.
        uint8_t stack_symbols[MAX_INSTRUCTIONS];
        for (uint8_t i = 0; i < MAX_INSTRUCTIONS; i++) {
            // UINT8_MAX is greater than the highest stack symbol value
            stack_symbols[i] = UINT8_MAX;
        }

        // cursor for current highest offset value
        uint8_t stack_idx = 0;

        for (uint8_t i = 1; i <= instructions; i++) {
            inst_t* inst = &function.instructions[instructions - i];
            inst->opcode = parse_val(fp, &offset, &buffer, OPCODE_SIZE);
            parse_inst(inst, fp, &buffer, &offset);
        }

        for (uint8_t i = 0; i < instructions - 1; i++) {
            map_stack(&function.instructions[i], stack_symbols, &stack_idx);
        }

        function.frame_size = stack_idx;
        function.label = parse_val(fp, &offset, &buffer, FUNC_SIZE);

        for (uint8_t i = 0; i < function_idx; i++) {
            if (function.label == functions[i].label) {
                errx(1, "multiple function definitions found with label %d",
                        function.label);
            }
        }

        functions[function_idx++] = function;
    }

    fclose(fp);
}

void parse_inst(inst_t* inst, FILE* fp, buf_t* buffer, long* offset) {
    switch (inst->opcode) {
    case MOV:
    case REF:
    case ADD:
        inst->arg1 = parse_arg(fp, buffer, offset);
        inst->arg2 = parse_arg(fp, buffer, offset);
        break;
    case CAL:
    case PRINT:
    case NOT:
    case EQU:
        inst->arg1 = parse_arg(fp, buffer, offset);
        break;
    case RET:
        break;
    }
}

arg_t parse_arg(FILE* fp, buf_t* buffer, long* offset) {
    arg_t arg;

    arg.type = parse_val(fp, offset, buffer, ARG_TYPE_SIZE);

    switch (arg.type) {
    case VAL:
        arg.value = parse_val(fp, offset, buffer, VALUE_SIZE);
        break;
    case REG:
        arg.value = parse_val(fp, offset, buffer, REG_SIZE);
        break;
    case STACK:
        arg.value = parse_val(fp, offset, buffer, STACK_SIZE);
        break;
    case PTR:
        arg.value = parse_val(fp, offset, buffer, PTR_SIZE);
        break;
    }

    return arg;
}

uint8_t parse_val(FILE* fp, long* offset, buf_t* buffer, const uint8_t length) {
    if (buffer->size < length) {
        // extend buffer
        *offset -= 1;
        if (*offset < 0)
            errx(1, "reached unexpected beginning of file while trying to "
                    "parse.");

        fseek(fp, *offset, SEEK_SET);
        uint8_t buf;
        fread(&buf, 1, 1, fp);

        buffer->data += buf << buffer->size;
        buffer->size += 8;
    }

    const uint8_t value = buffer->data & ((1 << length) - 1);
    buffer->data >>= length;
    buffer->size -= length;
    return value;
}

void map_stack(inst_t* inst, uint8_t* stack_symbols, uint8_t* stack_idx) {
    switch (inst->opcode) {
    case MOV:
    case REF:
    case ADD:
        map_symbol(&inst->arg1, stack_symbols, stack_idx);
        map_symbol(&inst->arg2, stack_symbols, stack_idx);
        break;
    case CAL:
    case PRINT:
    case NOT:
    case EQU:
        map_symbol(&inst->arg1, stack_symbols, stack_idx);
        break;
    case RET:
        break;
    }
}

void map_symbol(arg_t* arg, uint8_t* stack_symbols, uint8_t* stack_idx) {
    if (arg->type != STACK && arg->type != PTR)
        return;

    if (stack_symbols[arg->value] != UINT8_MAX) {
        arg->value = stack_symbols[arg->value];
        return;
    }

    stack_symbols[arg->value] = *stack_idx;
    arg->value = (*stack_idx)++;
}
