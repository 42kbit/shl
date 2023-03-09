#ifndef __H_USAGE_ELF_LINKER_SRC_SYSCALL_H
#define __H_USAGE_ELF_LINKER_SRC_SYSCALL_H

/* Raw syscall. Order may seem strange, but this way less instructions used.
 * Use syscall (and not _syscall) macro todo call with params inorder.
*/
long __syscall (
	unsigned long rdi,
	unsigned long rsi,
	unsigned long rdx,
	unsigned long r10,
	unsigned long r8,
	unsigned long r9,
	unsigned long callnr
);

#define syscall(callnr, rdi, rsi, rdx, r10, r8, r9)     \
	__syscall (rdi, rsi, rdx, r10, r8, r9, callnr)

static inline void sys_exit (int retcode){
	syscall (60, retcode, 0, 0, 0, 0, 0);
}

static inline unsigned long sys_write (unsigned int fd, const char* str, size_t slen){
	unsigned long retval = syscall (1, fd, (unsigned long)str, slen, 0, 0, 0);
	syscall (74, fd, 0, 0, 0, 0, 0);
	return retval;
}

#endif /* __H_USAGE_ELF_LINKER_SRC_SYSCALL_H */
