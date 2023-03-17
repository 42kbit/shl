/* Does a PIC stack pointer load, then jumps to main() */
#include <lib/cmn.h>
#include <lib/syscall.h>

#include "aux.h"

#define PAGE_SIZE 4096

int main(int argc, const char* argv[], const char* envp[]);

__attribute__((always_inline)) 
static inline void __set_sp (volatile void* new) {
	asm volatile ("leaq %0, %%rsp\n\t" : "=m"(new));
}

static char __proc_stack[mib(1)];
void dlmain(int argc, const char* argv[], const char* envp[]){
	__set_sp(__proc_stack);
	sys_exit (main(argc, argv, envp));
}