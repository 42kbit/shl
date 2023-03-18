
.globl __set_sp
__set_sp:
	/* return address */
	movq (%rsp), %rax
	movq %rdi, %rsp
	jmp *%rax