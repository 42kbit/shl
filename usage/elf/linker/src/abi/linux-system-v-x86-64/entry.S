/* Entry to x86-64 Linux System V ABI dynamic linker. */

#include <asm/unistd_64.h>
#include <linux/mman.h>

.extern exit
.extern pre_main

.section .text, "ax", @progbits
.globl _start

/* Default stack size could be get through syscalls,
 * But for now, this will do the job */
#define __STACK_SIZE 1024 * 1024

_start:
	xorq %rbx, %rbx

	movq (%rsp),  %rbx

	movq (%rsp),  %rdi
	leaq 8(%rsp), %rsi
	leaq 16(%rsp, %rbx, 8), %rdx
	

	/* job here is done, pre_main must exit on his own. */
	jmp pre_main