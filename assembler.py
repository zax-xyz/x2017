#!/usr/bin/env python3
import random
from math import ceil
from sys import argv, stdin, stdout

MAX_SYMBOLS = 1 << 5
OPCODES = {k: v for v, k in enumerate(('MOV', 'CAL', 'RET', 'REF', 'ADD',
                                       'PRINT', 'NOT', 'EQU'))}
ARGTYPES = {k: v for v, k in enumerate(('VAL', 'REG', 'STK', 'PTR'))}

stack_symbols = {}


def arg(type, val, bits):
    length = {'VAL': 8, 'REG': 3}.get(type, 5)

    if type in ('STK', 'PTR'):
        try:
            val = stack_symbols[val]
        except KeyError:
            tmp = random.choice(
                [i for i in range(MAX_SYMBOLS) if i not in stack_symbols.values()]
            )
            stack_symbols[val] = tmp
            val = tmp
    else:
        val = int(val)

    return (((bits << length) + val) << 2) + ARGTYPES[type], length + 2


def compile(file_in=stdin, file_out=stdout.buffer):
    bits = 0
    length = 0
    ops = 0
    is_first = True

    for line in file_in:
        line = line.split()
        op = line[0]

        if op == 'FUNC':
            if not is_first:
                bits = (bits << 5) + ops
                length += 5
            else:
                is_first = False

            label = int(line[-1])
            bits = (bits << 3) + label
            length += 3
            ops = 0
        else:
            ops += 1

            if op in ('MOV', 'REF', 'ADD'):
                bits, l = arg(line[3], line[4], bits)
                length += l
            if op != 'RET':
                bits, l = arg(line[1], line[2], bits)
                length += l

            bits = (bits << 3) + OPCODES[op]
            length += 3

    bits = (bits << 5) + ops
    length += 5

    length = ceil(length / 8)

    file_out.write(bits.to_bytes(length, 'big'))


if len(argv) > 1:
    with open(argv[1], 'r') as file_in, open(argv[2], 'wb') as file_out:
        compile(file_in, file_out)
else:
    compile()
