/*
 * _start function specific to AMD64 (x86-64) linux ABI
 *
 * %rsp usually stores the return address for the function, but since _start
 * isn't actually a function, it doesn't get put there, and instead argc ends up
 * taking its place since it's the first thing that gets put on the stack,
 * followed by argv
 */
asm(
    ".global _start\n"
    "_start:"
    "    movq   (%rsp), %rdi\n" // argc is in rsp (register stack pointer)
    "    leaq   8(%rsp), %rsi\n" // argv pointer
    "    call   main\n"

    "    movl   %eax, %edi\n" // ax register contains return value
    "    call   exit\n"
);
