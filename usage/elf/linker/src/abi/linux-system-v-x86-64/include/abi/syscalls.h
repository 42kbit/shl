#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SYSCALLS_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SYSCALLS_H

#define PROT_NONE	0x0
#define PROT_READ	0x1
#define PROT_WRITE	0x2
#define PROT_EXEC	0x4

#define MAP_PRIVATE	0x2
#define MAP_ANONYMOUS	0x20
#define MAP_FIXED	0x10

#define MAP_FAILED ((void*)-1)

void sys_exit (int retcode);

ssize_t sys_write (int fd, const void* str, size_t slen);

int sys_munmap(void *addr, size_t length);

int sys_open (const char* path, int flags, ...);

int sys_close (int fd);

ssize_t sys_read (int fd, void* buf, size_t count);

off_t sys_lseek (int fd, off_t offset, int whence);

void* sys_mmap (void* base, size_t size,
		unsigned long int prot,
		unsigned long int flags,
		int fd, off_t offset);

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SYSCALLS_H */
