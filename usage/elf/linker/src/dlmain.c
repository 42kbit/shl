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

/* Descriptor of a loaded in memory shared object. */
#define PHDR_MAX_LOAD 32
struct so_mem_desc {
	void* base;
	int64_t phdr_nent;
	struct elfw(phdr)
		*phdr, /* Pointer to dynamic program header */
		*phdr_dynamic, /* Dynamic program header entry */
		*phdr_load[PHDR_MAX_LOAD+1], /* NULL terminated array of pointers to LOAD headers */
		**phdr_load_top; /* Top of that array to keep size ok */
};

#define EMANYDYN	1
#define EPHDROVERFLOW	2
/* Initializes shared object memory descriptor from provided auxvals array of pointers.
 * e.g. auxvals[AT_PHDR] == NULL if no phdr is provided, or point to struct auxv otherwise.
*/
static inline int init_so_from_auxvals (struct so_mem_desc* p, struct auxv** auxvals) {
	p->phdr = auxvals[AT_PHDR]->a_un.a_ptr,
	/* The pointer itself is valid, cuz it points into ELF ehdr, but content of header itself
	 * is undefined (no word about it in System V ABI), so p->base is valid(?) for ELF version 1.
	*/
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

#define EMMAPFAILED 1
#define EREADFAILED 2
/* Memory maps ELF file descriptor. After setups descriptor. */
static inline int so_mmap_fd (struct so_mem_desc* p, unsigned int fd) {
	struct elfw(ehdr) ehdr;
	if (sys_read(fd, &ehdr, sizeof(ehdr)))
		return -EREADFAILED;
	return 0;
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

	/* Represents executable given by operating system on startup.
	 * LOAD type segments are already loaded.
	*/
	struct so_mem_desc aux_exec;
	int retval;
	if ((retval = init_so_from_auxvals (&aux_exec, auxvals)) < EOK){
		printf("Init from aux error!\n");
		return -retval;
	}

	printf("Got base: %p\n", aux_exec.base);

	/* Before giving control to executable, make sure to push argc, argv[] and envp[]
	 * according to System V ABI x86-64, but for now, we dont care.
	 */
	void (*entry)() = auxvals[AT_ENTRY]->a_un.a_ptr;
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