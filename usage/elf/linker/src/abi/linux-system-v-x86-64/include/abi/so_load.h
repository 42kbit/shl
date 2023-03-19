#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_LOAD_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_LOAD_H

#include <abi/so.h>

/* Memory maps ELF file descriptor. After setups descriptor. */
static inline int so_mmap_fd (const struct so_mem_desc* p, int fd) {
	/* TODO: ELF validation? just in case.
	 * Maybe do it only in debug
	*/
	struct elfw(ehdr) ehdr;
	read (fd, &ehdr, sizeof(struct elfw(ehdr)));
	
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
		lseek (fd, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET);
		read (fd, &phdr, sizeof(struct elfw(phdr)));

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
	for (struct elfw(dyn)** iter = p->dyn_needed; *iter != NULL; iter++)
	{
		struct shl_list_node* liter;
		struct libdir* liter_ent;
		shl_list_for_each_entry_auto (search_dirs, liter, liter_ent, list) {
			char filepath [FPATH_MAX] = {0};
			strncat (filepath, liter_ent->path, FPATH_MAX);
			strncat (filepath, "/", FPATH_MAX);
			strncat (filepath, so_strtab_off(p, (*iter)->d_un.d_val), FPATH_MAX);
			int fd = open (filepath, O_RDONLY);
			if (fd < 0)
				continue;
			printf ("Dependency %s exists!\n", filepath);
			close (fd);
		}
	}
	return EOK;
}

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_LOAD_H */
