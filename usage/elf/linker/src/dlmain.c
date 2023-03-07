#include <errno.h>
#include <stddef.h>
#include "../../../../shl_elf.h"

#include "aux.h"
#include "syscall.h"
#include "io.h"

static inline void sys_exit (int retcode){
	syscall (60, retcode, 0, 0, 0, 0, 0);
}

int main(int argc, const char* argv[], const char* envp[]){
	printf("Dynamic Linker started\n");

	/* Seek end of envp[] */
	const char** iter;
	for (iter = envp; *iter != NULL; iter++)
		;
	struct auxv	*auxp = (struct auxv*)(iter + 1),
			*auxvals[AT_NTYPES] = {NULL};
	for (struct auxv* iter = auxp;
		iter->a_type != AT_NULL;
		iter++)
	{
		if (is_valid_atype(iter)){
			auxvals[iter->a_type] = iter;
		}
	}

	printf ("AT_PHDR = %p\n", auxvals[AT_PHDR]->a_un.a_val);

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