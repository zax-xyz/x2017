from sys import stdin, stdout

OPCODES = ['MOV', 'CAL', 'RET', 'REF', 'ADD', 'PRINT', 'NOT', 'EQU']
ARG_TYPES = ['VAL', 'REG', 'STK', 'PTR']

def b(i, l):
    return format(i, f'0{l}b')

def arg(type, val):
    if type == "VAL":
        l = 8
    elif type == "REG":
        l = 3
    else:
        l = 5

    if type == "STK":
        val = ord(val) - ord('A')
    else:
        val = int(val)

    return b(val, l) + b(ARG_TYPES.index(type), 2)

s = ''

for line in stdin:
    line = line.split()
    op = line[0]
    if op == "FUNC":
        label = int(line[-1])
        s += b(label, 3)
        ops = 0
    else:
        ops += 1
        if op in ["MOV", "REF", "ADD"]:
            s += arg(line[3], line[4])
        if op != "RET":
            s += arg(line[1], line[2])

        s += b(OPCODES.index(op), 3)

        if op == "RET":
            s += b(ops, 5)

if len(s) % 8:
    s = '0' * (8 - (len(s) % 8)) + s

stdout.buffer.write(int(s, 2).to_bytes(len(s) // 8, 'big'))
