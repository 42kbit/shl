/* Entry to x86-64 Linux System V ABI dynamic linker. */

.extern _exit
.extern pre_main

.section .text, "ax", @progbits
.globl _start
_start:
	xorq %rbx, %rbx

	movq (%rsp),  %rax

	movq (%rsp),  %rdi
	leaq 8(%rsp), %rsi
	leaq 16(%rsp, %rax, 8), %rdx

	/* job here is done, dlmain must exit on his own. */
	jmp pre_main