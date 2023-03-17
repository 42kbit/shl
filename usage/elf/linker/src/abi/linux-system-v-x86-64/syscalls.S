
.globl __syscall
__syscall:
	/* rdi, rsi, rdx was set by calling convention. */
	movq %rcx, %r10
	/* r8, r9 was set by calling convention. */

	/* Lastly, use callnr from stack */
	movq 8(%rsp), %rax
	syscall
	ret