#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_H

#include <shl_elf.h>
#include <shl_list.h>
#include <lib/string.h>
#include <lib/io.h>
#include <stdbool.h>

#include <errno.h>

/* Descriptor of a loaded in memory shared object. */
#define PHDR_MAX_LOAD 32
#define DYN_MAX_DEPS 32
struct so_mem_desc {
	void	*base,
		**got;
	int64_t	phdr_nent;
	struct elfw(phdr)
		*phdr, /* Pointer to dynamic program header */
		*phdr_dynamic, /* Dynamic program header entry */
		*phdr_load[PHDR_MAX_LOAD+1], /* NULL terminated array of pointers to LOAD headers */
		**phdr_load_top; /* Top of that array to keep size ok */
	struct elfw(dyn)
		*dyn_strtab, /* Points to string table entry in dynamic array */
		*dyn_symtab, /* Points to symbol table entry in dynamic array */
		*dyn_needed[DYN_MAX_DEPS+1],
		**dyn_needed_top;
};

struct libdir{
	const char* path;
	struct shl_list_node list;
};

static inline struct elfw(dyn)* so_dyn_addr (struct so_mem_desc* p){
	return ptradd (p->base, p->phdr_dynamic->p_vaddr);
}
static inline struct elfw(sym)* so_symt_addr (struct so_mem_desc* p){
	return ptradd (p->base, p->dyn_symtab->d_un.d_ptr);
}
static inline const char* so_strtab_off (struct so_mem_desc* p, unsigned int offset) {
	const char * strtab = ptradd(p->base, p->dyn_strtab->d_un.d_ptr);
	return strtab + offset;
}

static inline int init_so_mem_desc (
			struct so_mem_desc* p,
			struct elfw(phdr)* phdr,
			elfw(half) phnum)
{
	memzero (p);

	p->phdr = phdr,
	/* The pointer itself is valid, cuz it points into ELF ehdr, but content of header itself
	 * is undefined (no word about it in System V ABI), so p->base is valid(?) for ELF version 1.
	*/
	p->base = (char*)p->phdr - sizeof(struct elfw(ehdr));
	p->phdr_nent		= phnum;
	p->phdr_load_top	= p->phdr_load;
	p->dyn_needed_top	= p->dyn_needed;

	long int loaded = 0;
	for (	struct elfw(phdr)* iter = p->phdr;
		(iter - p->phdr) < p->phdr_nent;
		iter++)
	{
		switch(iter->p_type){
		case PT_LOAD:
			loaded = (p->phdr_load_top - p->phdr_load);
			if (loaded > PHDR_MAX_LOAD)
				return -EOVERFLOW;
			*(p->phdr_load_top++)	= iter;
			*(p->phdr_load_top)	= NULL; /* Keep it NULL terminated */
			continue;
		case PT_DYNAMIC:
			if(p->phdr_dynamic != NULL)
				return -EOVERFLOW;
			p->phdr_dynamic = iter;
			continue;
		}
	}
	
	if (p->phdr_dynamic == NULL)
		return -EINVAL;

	/* Now, parse dynamic segment */
	for (	struct elfw(dyn)* iter = so_dyn_addr(p);
		iter->d_tag != DT_NULL;
		iter++)
	{
		switch (iter->d_tag){
		case DT_NEEDED:
			loaded = (p->dyn_needed_top - p->dyn_needed);
			if (loaded > DYN_MAX_DEPS)
				return -EINVAL;
			*(p->dyn_needed_top++)	= iter;
			*(p->dyn_needed_top) 	= NULL;
			continue;
		case DT_STRTAB:
			if (p->dyn_strtab != NULL)
				return -EINVAL;
			p->dyn_strtab = iter;
			continue;
		case DT_SYMTAB:
			if (p->dyn_symtab != NULL)
				return -EINVAL;
			p->dyn_symtab = iter;
			continue;
		case DT_PLTGOT:
			if (p->got != NULL)
				return -EINVAL;
			p->got = ptradd(p->base, iter->d_un.d_ptr);
			continue;
		}
	}

	return EOK;
}

/* Memory maps ELF file descriptor. After setups descriptor. */
static inline int so_mmap_fd (const struct so_mem_desc* p, int fd) {
	/* TODO: ELF validation? just in case.
	 * Maybe do it only in debug
	*/
	struct elfw(ehdr) ehdr;
	sys_read (fd, &ehdr, sizeof(struct elfw(ehdr)));
	
	/* Total allcation size, this includes:
	 * ELF Header
	 * Program headers
	 * LOAD segments
	 */
	size_t btotal = 0;
	char * minaddr = NULL;
	char * maxaddr = NULL;
	/* Append btotal and find min address, that would be used as base. */
	for (int i = 0; i < ehdr.e_phnum; i++){
		struct elfw(phdr) phdr;
		sys_lseek (fd, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET);
		sys_read (fd, &phdr, sizeof(struct elfw(phdr)));

		if (phdr.p_type != PT_LOAD)
			continue;
		minaddr = (char*)MIN((addr_t)minaddr, (addr_t)phdr.p_vaddr);
		maxaddr = (char*)MAX((addr_t)maxaddr, (addr_t)ptradd(phdr.p_vaddr, phdr.p_memsz));
	}
	btotal += maxaddr - minaddr;
	printf("%p, %p, %h\n", minaddr, maxaddr, btotal);

	return EOK;
}

/* Returns error code.
 * If fails, filename string pointer is written to *failname
 */
#define FPATH_MAX 0x1000
static inline int load_so_deps (
			struct so_mem_desc* p,
			struct shl_list_node* search_dirs,
			const char** failname)
{
	for (	struct elfw(dyn)** iter = p->dyn_needed;
		*iter != NULL;
		iter++)
	{
		struct shl_list_node* liter;
		struct libdir* liter_ent;
		shl_list_for_each_entry_auto (search_dirs, liter, liter_ent, list) {
			char filepath [FPATH_MAX] = {0};
			strncat (filepath, liter_ent->path, FPATH_MAX);
			strncat (filepath, "/", FPATH_MAX);
			strncat (filepath, so_strtab_off(p, (*iter)->d_un.d_val), FPATH_MAX);
			int fd = sys_open (filepath, O_RDONLY, 0);
			if (fd < 0)
				continue;
			printf ("Dependency %s exists!\n", filepath);
			sys_close (fd);
		}
	}
	return EOK;
}

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_H */
