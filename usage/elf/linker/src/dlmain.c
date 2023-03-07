#include <errno.h>
#include <stddef.h>
#include "aux.h"
#include "../../../../shl_elf.h"

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

typedef unsigned long size_t;

#define syscall(callnr, rdi, rsi, rdx, r10, r8, r9)     \
	__syscall (rdi, rsi, rdx, r10, r8, r9, callnr)

static inline void sys_exit (int retcode){
	syscall (60, retcode, 0, 0, 0, 0, 0);
}

static inline void reverse_inplace(char * str, int len){
	char tmp;
	for (int i = 0; i < len/2; i++){
		tmp = str[i];
		str[i] = str[len-i-1];
		str[len-i-1] = tmp;
	}
}
static inline int itoan (int n, int base, char* buf){
	if (base <= 0)
		return -EINVAL;
	static char __itoan_nums[] = "0123456789abcdef";
	int i = 0;
	for (; n != 0; n /= base, i++){
		int digit = n % base;
		buf[i] = __itoan_nums[digit];
	}
	reverse_inplace (buf, i);
	buf[i+1] = '\0';
	return i;
}

static inline unsigned long sys_write (unsigned int fd, const char* str, size_t strlen){
	unsigned long retval = syscall (1, fd, (unsigned long)str, strlen, 0, 0, 0);
	syscall (74, fd, 0, 0, 0, 0, 0);
	return retval;
}
static inline void putc (char c){
	sys_write (1, &c, 1);
}
static inline void puts (const char* str){
	char c;
	while ((c = *(str++)) != '\0' && c != '\n')
		putc(c);
	putc('\n');
}

int main(int argc, const char* argv[], const char* envp[]){
	char buf[70] = {0};
	itoan (argc, 10, buf);

	/* Seek end of envp[] */
	const char** iter;
	for (iter = envp; *iter != NULL; iter++)
		;

	struct auxv * auxp = (struct auxv*)(iter + 1);
	for (struct auxv* iter = auxp;
		iter->a_type != AT_NULL;
		iter++)
	{
		itoan (iter->a_type, 16, buf);
		puts(buf);
		puts(a_val_str(iter->a_type));
	}

	return 0;
}

__attribute__((always_inline)) 
static inline void __set_sp (volatile void* new) {
	asm volatile ("leaq %0, %%rsp\n\t" : "=m"(new));
}

static char __proc_stack[1024];
void dlmain(int argc, const char* argv[], const char* envp[]){
	__set_sp(__proc_stack);
	sys_exit (main(argc, argv, envp));
}