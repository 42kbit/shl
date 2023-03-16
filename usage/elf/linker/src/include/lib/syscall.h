#ifndef __H_USAGE_ELF_LINKER_SRC_SYSCALL_H
#define __H_USAGE_ELF_LINKER_SRC_SYSCALL_H

#include <asm/unistd.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

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

static inline void* sys_mmap(void *addr, size_t length, int prot, int flags,
		int fd, off_t offset)
{
	return (void*) syscall (__NR_mmap, addr, length, prot, flags, fd, offset);
}

static inline int sys_munmap(void *addr, size_t length)
{
	return (int) syscall (__NR_munmap, addr, length, 0, 0, 0, 0);
}

static inline int sys_open (const char* path, int flags, mode_t mode) {
	return (int) syscall (__NR_open, path, flags, mode, 0, 0, 0);
}

static inline int sys_close (int fd) {
	return (int) syscall (__NR_close, fd, 0, 0, 0, 0, 0);
}

static inline int sys_read (unsigned int fd, void* buf, size_t count){
	return (int) syscall (__NR_read, fd, buf, count, 0, 0, 0);
}

static inline off_t sys_lseek (unsigned int fd, off_t offset, unsigned int origin) {
	return (off_t) syscall (__NR_lseek, fd, offset, origin, 0, 0, 0);
}

#endif /* __H_USAGE_ELF_LINKER_SRC_SYSCALL_H */
