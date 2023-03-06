
#define EINVAL 22 /* invalid value */

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

static inline unsigned long sys_write (unsigned int fd, const char* str, size_t strlen){
	unsigned long retval = syscall (1, fd, (unsigned long)str, strlen, 0, 0, 0);
	syscall (74, fd, 0, 0, 0, 0, 0);
	return retval;
}

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
	return i;
}

int main(int argc, const char* argv[], const char* envp[]){
	char buf[55] = {0};
	itoan (1234, 10, buf);

	sys_write(1, buf, 5);
	sys_write(1, "\n", 1);

	return 0;
}

static char __proc_stack[1024];
void dlmain(int argc, const char* argv[], const char* envp[]){
	asm volatile ("leaq %0, %%rsp\n\t" : "=m"(__proc_stack));
	sys_exit (main(argc, argv, envp));
}

