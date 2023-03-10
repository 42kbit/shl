#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include "../../../../shl_elf.h"

#include "aux.h"
#include "syscall.h"
#include "io.h"
#include "cmn.h"

/* Danke, https://stackoverflow.com/a/1505631
 * Preprocessor time check whether CPU is 32 or 64 bit.
 * 
 * This shit linker probably wont support 32 bits ever, but who knows, maybe one day...
 * If so, 32 bit linker would support
*/

/* Check GCC */
#if __GNUC__
	#if __x86_64__ || __ppc64__
		#define BITS64
	#else
		#define BITS32
	#endif
#endif

#if defined(BITS32)
	#define elfw(expr) elf32_ ## expr
#elif defined(BITS64)
	#define elfw(expr) elf64_ ## expr
#else
	#error BITS32 and BITS64 are not defined
#endif

#define PAGE_SIZE 4096

/* Represents loaded shared object. */
#define PHDR_MAX_LOAD 32
struct so {
	void* base;
	int64_t phdr_nent;
	struct elfw(phdr)* phdr,
		/* Pointer to dynamic program header */
		*phdr_dynamic,
		/* NULL terminated array of pointers to LOAD headers
		 * Top of that array to keep size ok
		 */
		*phdr_load[PHDR_MAX_LOAD+1],
		**phdr_load_top;
};

#define EMANYDYN	1
#define EPHDROVERFLOW	2
static inline int init_so_from_auxvals (struct so* p, struct auxv** auxvals) {
	p->phdr = auxvals[AT_PHDR]->a_un.a_ptr,
	p->base = (char*)p->phdr - sizeof(struct elfw(ehdr));
	p->phdr_nent = auxvals[AT_PHNUM]->a_un.a_val;
	p->phdr_load_top = p->phdr_load;
	for (	struct elfw(phdr)* iter = p->phdr;
		(iter - p->phdr) < p->phdr_nent;
		iter++)
	{
		switch(iter->p_type){
			case PT_DYNAMIC:
				if(p->phdr_dynamic != NULL){
					return -EMANYDYN;
				}
				p->phdr_dynamic = iter;
				continue;
			case PT_LOAD:
				long int loaded = (p->phdr_load_top - p->phdr_load);
				if (loaded > PHDR_MAX_LOAD) {
					return -EPHDROVERFLOW;
				}
				*(p->phdr_load_top++) = iter;
				*(p->phdr_load_top) = NULL; /* Keep it NULL terminated */
				continue;
		}
	}

	return EOK;
}

/* Loads all segments from phdrs that typed LOAD
 * Writes load address in pbase
 * Returns error
*/
#define EMMAPFAILED 1
static inline int so_mmap_segments (struct so* p, void** pbase) {
	char *begin = NULL, *end = NULL;
	for (	struct elfw(phdr)** iter = p->phdr_load;
		*iter != NULL;
		iter++)
	{
		begin	= MIN(begin, (char*)(*iter)->p_vaddr);
		end	= MAX(end, (char*)(*iter)->p_vaddr);
	}
	/* Reserve base address for shared object */
	char* base = sys_mmap(
		NULL,
		end - begin,
		PROT_NONE,
		MAP_PRIVATE | MAP_ANON,
		-1, 0);
	if (base == MAP_FAILED){
		return -EMMAPFAILED;
	}

	for (	struct elfw(phdr)** iter = p->phdr_load;
		*iter != NULL;
		iter++)
	{
		const int alignment = (*iter)->p_align;
		char 	*i_begin = base +
				(addr_t)palign_low((*iter)->p_paddr, alignment),
			*i_end	 = base +
				(addr_t)palign((*iter)->p_paddr + (*iter)->p_memsz, alignment);
		int prot = 0;
		prot |= (*iter)->p_flags & PF_R ? PROT_READ : 0;
		prot |= (*iter)->p_flags & PF_W ? PROT_WRITE: 0;
		prot |= (*iter)->p_flags & PF_X ? PROT_EXEC : 0;
		char* i_sect = sys_mmap(
			i_begin,
			i_end - i_begin,
			prot,
			MAP_PRIVATE | MAP_FIXED | MAP_ANON,
			-1, 0);
		if (i_sect != i_begin){
			return -EMMAPFAILED;
		}
	}
	*pbase = base;
	return EOK;
}

int main(int argc, const char* argv[], const char* envp[]){
	printf("Linux x86-64 dynamic linker started\n");

	/* Seek end of envp[] */
	const char** iter;
	for (iter = envp; *iter != NULL; iter++)
		;
	/* Write auxv into auxvals to easy access. */
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

	/* Represents executable given by operating system on startup. */
	struct so aux_exec;
	int retval;
	if ((retval = init_so_from_auxvals (&aux_exec, auxvals)) < EOK){
		printf("Init from aux error!\n");
		return -retval;
	}

	/*
	for (	struct elfw(phdr)** iter = aux_exec.phdr_load;
		*iter != NULL;
		iter++)
	{
		printf("%p\n", (char*)aux_exec.base + (addr_t)(*iter)->p_vaddr);
	}
	*/
	void* aux_exec_base;
	if ((retval = so_mmap_segments(&aux_exec, &aux_exec_base))) {
		printf("Failed to mmap segment\n");
		return -retval;
	}

	if (aux_exec.phdr_dynamic == NULL){
		printf("PT_DYNAMIC header missing! Exiting..\n");
		return -1;
	}

	printf("Got base: %p\n", aux_exec.base);

	void (*entry)() = auxvals[AT_ENTRY]->a_un.a_ptr;
	printf("Got entrypoint: %p\n", entry);
	entry();

	return EOK;
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