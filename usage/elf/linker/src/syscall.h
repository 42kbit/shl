#ifndef __H_USAGE_ELF_LINKER_SRC_SYSCALL_H
#define __H_USAGE_ELF_LINKER_SRC_SYSCALL_H

#include <asm/unistd.h>
#include <sys/mman.h>

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
	__syscall ((unsigned long)rdi,			\
		(unsigned long)rsi,			\
		(unsigned long)rdx,			\
		(unsigned long)r10,			\
		(unsigned long)r8,			\
		(unsigned long)r9,			\
		(unsigned long)callnr)

static inline void sys_exit (int retcode){
	syscall (__NR_exit, retcode, 0, 0, 0, 0, 0);
}

static inline unsigned long sys_write (unsigned int fd, const char* str, size_t slen){
	unsigned long retval = syscall (__NR_write, fd, str, slen, 0, 0, 0);
	syscall (__NR_fsync, fd, 0, 0, 0, 0, 0);
	return retval;
}

static inline void *sys_mmap(void *addr, size_t length, int prot, int flags,
		int fd, off_t offset)
{
	return (void*) syscall (__NR_mmap, addr, length, prot, flags, fd, offset);
}

static inline int sys_munmap(void *addr, size_t length)
{
	return (int) syscall (__NR_munmap, addr, length, 0, 0, 0, 0);
}

#endif /* __H_USAGE_ELF_LINKER_SRC_SYSCALL_H */
