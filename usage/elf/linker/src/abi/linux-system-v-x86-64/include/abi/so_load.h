#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H

#include <abi/so.h>
#include <abi/page.h>

/* Finds Min and Max load addresses. */
static inline int __phdr_find_load_bounds_from_fd (int fd, void** min, void** max)
{
	/* Read ehdr to stack */
	struct elfw(ehdr) ehdr;
	read (fd, &ehdr, sizeof(struct elfw(ehdr)));
	
	/* Iterate over phdrs, finding min and max load address */
	for (int i = 0; i < ehdr.e_phnum; i++){
		struct elfw(phdr) phdr;
		lseek (fd, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET);
		read (fd, &phdr, sizeof(struct elfw(phdr)));

		if (phdr.p_type != PT_LOAD)
			continue;
		void* lower = ptralign(phdr.p_vaddr, PAGE_SIZE);
		void* upper = ptralign(ptradd(phdr.p_vaddr, phdr.p_memsz), PAGE_SIZE);
		*min = (void*)MIN((addr_t)*min, (addr_t)lower);
		*max = (void*)MAX((addr_t)*max, (addr_t)upper);
	}
	return EOK;
}

/* Memory maps ELF file descriptor. After setups descriptor, pointed by dst */
static inline int so_mmap_fd (struct so_mem_desc* dst, const struct so_mem_desc* p, int fd) {
	char* load_max = NULL;
	char* load_min = NULL;
	if (__phdr_find_load_bounds_from_fd(fd, (void**)&load_min, (void**)&load_max) < EOK)
		return -EINVAL;
	if (load_min > load_max)
		return -EINVAL;
	printf ("%p, %p\n", load_min, load_max);
	return EOK;
}

/* Returns error code.
 * If fails, filename string pointer is written to *failname
 */
#define FPATH_MAX 0x1000
static inline int so_load_deps (
			struct so_mem_desc* p,
			struct shl_list_node* search_dirs,
			const char** failname)
{
	for (struct elfw(dyn)** iter = p->dyn_needed; *iter != NULL; iter++)
	{
		struct shl_list_node* liter;
		struct libdir* liter_ent;
		shl_list_for_each_entry_auto (search_dirs, liter, liter_ent, list) {

			char filepath [FPATH_MAX] = {0};
			const char* filename = so_mem_strtab_off(p, (*iter)->d_un.d_val);

			/* Build path */
			strncat (filepath, liter_ent->path, FPATH_MAX);
			strncat (filepath, "/", FPATH_MAX);
			strncat (filepath, filename, FPATH_MAX);

			int fd = open (filepath, O_RDONLY);
			if (fd < 0)
				continue;
			/* If file exists, load it into memory, build mem descriptor, then exit */
			struct so_mem_desc dummy;
			if (so_mmap_fd (&dummy, p, fd) < EOK) {
				close (fd);
				*failname = filename;
				return -EINVAL;
			}
			close (fd);
		}
	}
	return EOK;
}

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H */
