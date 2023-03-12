#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include "../../../../shl_elf.h"
#include "../../../../shl_list.h"

#include "string.h"
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
#define DYN_MAX_DEPS 32
struct so_mem_desc {
	void* base;
	int64_t phdr_nent;
	struct elfw(phdr)
		*phdr, /* Pointer to dynamic program header */
		*phdr_dynamic, /* Dynamic program header entry */
		*phdr_load[PHDR_MAX_LOAD+1], /* NULL terminated array of pointers to LOAD headers */
		**phdr_load_top; /* Top of that array to keep size ok */
	struct elfw(dyn)
		*dyn_strtab, /* Points to string table entry in dynamic array */
		*dyn_needed[DYN_MAX_DEPS+1],
		**dyn_needed_top;
};

static inline void init_so_mem_desc (struct so_mem_desc* p){
	memset(p, 0, sizeof(*p));
}

static inline struct elfw(dyn)* so_dyn_addr (struct so_mem_desc* p){
	return ptradd (p->base, p->phdr_dynamic->p_vaddr);
}

static inline const char* so_strtab_off (struct so_mem_desc* p, unsigned int offset) {
	const char * strtab = ptradd(p->base, p->dyn_strtab->d_un.d_ptr);
	return strtab + offset;
}

struct libdir{
	const char* path;
	struct shl_list_node list;
};

struct shl_list_node libdirs_head;

#define ENODYN			1
#define EMANYDYN		2
#define EPHDROVERFLOW		3
#define EDYNDEPSOVERFLOW	4
#define EMANYSTRTAB		5

/* Initializes shared object memory descriptor from provided auxvals array of pointers.
 * e.g. auxvals[AT_PHDR] == NULL if no phdr is provided, or point to struct auxv otherwise.
*/
static inline int init_so_from_auxvals (struct so_mem_desc* p, struct auxv** auxvals) {
	long int loaded = 0;

	init_so_mem_desc(p);

	p->phdr = auxvals[AT_PHDR]->a_un.a_ptr,
	/* The pointer itself is valid, cuz it points into ELF ehdr, but content of header itself
	 * is undefined (no word about it in System V ABI), so p->base is valid(?) for ELF version 1.
	*/
	p->base = (char*)p->phdr - sizeof(struct elfw(ehdr));
	p->phdr_nent		= auxvals[AT_PHNUM]->a_un.a_val;
	p->phdr_load_top	= p->phdr_load;
	p->dyn_needed_top	= p->dyn_needed;
	for (	struct elfw(phdr)* iter = p->phdr;
		(iter - p->phdr) < p->phdr_nent;
		iter++)
	{
		switch(iter->p_type){
		case PT_LOAD:
			loaded = (p->phdr_load_top - p->phdr_load);
			if (loaded > PHDR_MAX_LOAD)
				return -EPHDROVERFLOW;
			*(p->phdr_load_top++)	= iter;
			*(p->phdr_load_top)	= NULL; /* Keep it NULL terminated */
			continue;
		case PT_DYNAMIC:
			if(p->phdr_dynamic != NULL)
				return -EMANYDYN;
			p->phdr_dynamic = iter;
			continue;
		}
	}
	
	if (p->phdr_dynamic == NULL)
		return -ENODYN;

	/* Now, parse dynamic segment */
	for (	struct elfw(dyn)* iter = so_dyn_addr(p);
		iter->d_tag != DT_NULL;
		iter++)
	{
		switch (iter->d_tag){
		case DT_NEEDED:
			loaded = (p->dyn_needed_top - p->dyn_needed);
			if (loaded > DYN_MAX_DEPS)
				return -EDYNDEPSOVERFLOW;
			*(p->dyn_needed_top++)	= iter;
			*(p->dyn_needed_top) 	= NULL;
			continue;
		case DT_STRTAB:
			if (p->dyn_strtab != NULL)
				return -EMANYSTRTAB;
			p->dyn_strtab = iter;
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

/* Returns error code.
 * If fails, filename string pointer is written to *failname
 */
static inline int load_so_deps (struct so_mem_desc* p, const char** failname){
	for (	struct elfw(dyn)** iter = p->dyn_needed;
		*iter != NULL;
		iter++)
	{
		printf("%s\n", so_strtab_off(p, (*iter)->d_un.d_val));
	}
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

	const struct auxv* auxpg = auxvals[AT_PAGESZ];
	if(auxpg != NULL && auxpg->a_un.a_val != PAGE_SIZE){
		printf("Invalid page size. Hardcoded %u, actual %u",
			PAGE_SIZE, auxpg->a_un.a_val);
		return -1;
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
	
	/* Init path */
	shl_list_init_head (&libdirs_head);
		
	struct libdir default_path = {
		.path = "/lib"
	};
	shl_list_init_node (&(default_path.list));
	shl_list_insert_safe (&libdirs_head, &(default_path.list));

	struct libdir library_path = {
		.path = getenv(envp, "LD_LIBRARY_PATH")
	};
	shl_list_init_node (&(library_path.list));
	shl_list_insert_safe (&libdirs_head, &(library_path.list));
	
	struct shl_list_node* liter;
	struct libdir* liter_ent;
	shl_list_for_each_entry_prev_auto((&libdirs_head), liter, liter_ent, list){
		printf("%s\n", liter_ent->path);
	}
	
	const char* failname = NULL;
	if ((retval = load_so_deps (&aux_exec, &failname)) < EOK){
		printf("Dependency %s load error!\n", failname);
		return retval;
	}

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