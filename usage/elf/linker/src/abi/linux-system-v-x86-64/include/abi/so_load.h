#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H

#include <abi/so.h>
#include <abi/page.h>

#include <abi/syscalls.h>

static inline int __read_ehdr (struct elfw(ehdr)* ehdr, int fd) {
	if (sys_lseek (fd, 0, SEEK_SET) < 0)
		return -EINVAL;
	if (sys_read (fd, ehdr, sizeof(struct elfw(ehdr))) < 0)
		return -EINVAL;
	return EOK;
}

static inline int __read_phdr (struct elfw(phdr)* dst, struct elfw(ehdr)* ehdr,
			       int fd, int idx)
{
	if (sys_lseek (fd, ehdr->e_phoff + idx * ehdr->e_phentsize, SEEK_SET) < 0)
			return -EINVAL;
	if (sys_read (fd, dst, sizeof(struct elfw(phdr))) < 0)
			return -EINVAL;
	return EOK;
}

/* Finds Min and Max load addresses. */
static inline int __phdr_find_load_bounds_from_fd (int fd, void** min, void** max)
{
	/* Read ehdr to stack */
	struct elfw(ehdr) ehdr;
	__read_ehdr (&ehdr, fd);
	
	/* Iterate over phdrs, finding min and max load address */
	for (int i = 0; i < ehdr.e_phnum; i++){
		struct elfw(phdr) phdr;
		__read_phdr (&phdr, &ehdr, fd, i);

		if (phdr.p_type != PT_LOAD)
			continue;
		void* lower = ptralign(phdr.p_vaddr, PAGE_SIZE);
		void* upper = ptralign(ptradd(phdr.p_vaddr, phdr.p_memsz), PAGE_SIZE);
		*min = (void*)MIN((addr_t)*min, (addr_t)lower);
		*max = (void*)MAX((addr_t)*max, (addr_t)upper);
	}
	return EOK;
}

static inline int __phdr_mmap_load (void* _base, int fd) {
	char* base = _base;

	/* Read ehdr to stack */
	struct elfw(ehdr) ehdr;
	__read_ehdr (&ehdr, fd);
	
	/* Iterate over phdrs, finding min and max load address */
	for (int i = 0; i < ehdr.e_phnum; i++){
		struct elfw(phdr) phdr;
		__read_phdr (&phdr, &ehdr, fd, i);
		
		elfw(off) offset = align(phdr.p_offset, PAGE_SIZE);

		if (phdr.p_type != PT_LOAD)
			continue;
		int prot = 	(phdr.p_flags & PF_R ? PROT_READ  : 0) |
				(phdr.p_flags & PF_W ? PROT_WRITE : 0) |
				(phdr.p_flags & PF_X ? PROT_EXEC  : 0) ;
		void* sect = sys_mmap (base + phdr.p_vaddr, phdr.p_memsz, prot,
				   MAP_PRIVATE | MAP_FIXED, fd, offset);
		if (sect == MAP_FAILED)
			return -EINVAL;
	}
	return EOK;
}

/* Memory maps ELF file descriptor. After setups descriptor, pointed by dst */
static inline int so_mmap_fd (struct so_mem_desc* dst, const struct so_mem_desc* p, int fd) {
	char* load_max = NULL;
	char* load_min = NULL;
	if (__phdr_find_load_bounds_from_fd(fd, (void**)&load_min, (void**)&load_max) < EOK)
		return -EINVAL;
	size_t alloc_size = load_max - load_min;
	/* Map memory that could hold all load segments. */
	dst->base = sys_mmap (	NULL, alloc_size,
				PROT_NONE,
				MAP_PRIVATE | MAP_ANONYMOUS,
				-1, 0);
	if (dst->base == MAP_FAILED)
		return -ENOMEM;
	if (__phdr_mmap_load (dst->base, fd) < 0)
		return -ENOMEM;
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

			int fd = sys_open (filepath, O_RDONLY);
			if (fd < 0)
				continue;
			/* If file exists, load it into memory, build mem descriptor, then exit */
			struct so_mem_desc dummy;
			if (so_mmap_fd (&dummy, p, fd) < EOK) {
				sys_close (fd);
				*failname = filename;
				return -EINVAL;
			}
			sys_close (fd);
		}
	}
	return EOK;
}

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H */
