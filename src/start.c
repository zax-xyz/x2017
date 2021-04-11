// _start function specific to AMD64 (x86-64) linux ABI
asm(
    ".global _start\n"
    "_start:"
    "    movq   (%rsp), %rdi\n" // argc is in rsp (register stack pointer)
    "    leaq   8(%rsp), %rsi\n" // argv pointer
    "    call   main\n"

    "    movl   %eax, %edi\n" // ax register contains return value
    "    call   exit\n"
);
