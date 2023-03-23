/* This file is responsible for loading shared objects into memory, and provide
 * memory descriptors to operate with them.
*/
#include <abi/so_load.h>
#include <abi/so.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

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
			return -E2BIG;
	if (sys_read (fd, dst, sizeof(struct elfw(phdr))) < 0)
			return -EIO;
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
		void* lower = ptralign_low(phdr.p_vaddr, PAGE_SIZE);
		void* upper = ptralign(ptradd(phdr.p_vaddr, phdr.p_memsz), PAGE_SIZE);
		*min = (void*)MIN((addr_t)*min, (addr_t)lower);
		*max = (void*)MAX((addr_t)*max, (addr_t)upper);
	}
	return EOK;
}

static inline int __phdr_mmap_load (struct so_mem_desc* dst, int fd) {
	char* base = dst->base;

	/* Read ehdr to stack */
	struct elfw(ehdr) ehdr;
	__read_ehdr (&ehdr, fd);
	
	void* dynamic = NULL;
	
	/* Iterate over phdrs, finding min and max load address */
	for (int i = 0; i < ehdr.e_phnum; i++){
		struct elfw(phdr) phdr;
		__read_phdr (&phdr, &ehdr, fd, i);
		

		if (phdr.p_type == PT_DYNAMIC) {
			dynamic = ptradd (base, phdr.p_vaddr);
			continue;
		}
		if (phdr.p_type != PT_LOAD)
			continue;
		
		elfw(off) offset = align_low(phdr.p_offset, PAGE_SIZE);
		int prot = 	(phdr.p_flags & PF_R ? PROT_READ  : 0) |
				(phdr.p_flags & PF_W ? PROT_WRITE : 0) |
				(phdr.p_flags & PF_X ? PROT_EXEC  : 0) ;

		char* lower = ptralign_low(phdr.p_vaddr, PAGE_SIZE);
		char* upper = ptralign(ptradd(phdr.p_vaddr, phdr.p_memsz), PAGE_SIZE);
		size_t segsize = upper - lower;
		void* sect = sys_mmap (ptradd(base, lower), segsize, prot,
				   MAP_PRIVATE | MAP_FIXED, fd, offset);
		if (sect == MAP_FAILED)
			return -EINVAL;
		ptrdiff_t diff = phdr.p_memsz - phdr.p_filesz;
		if (diff > 0) {
			/* Zero memory left */
			memset (base + phdr.p_vaddr + phdr.p_memsz, 0, diff);
		}
	}
	if (!dynamic)
		return -EINVAL;
	if (so_mem_init_dynamic (dst, dynamic) < EOK)
		return -EINVAL;
	return EOK;
}


#define FPATH_MAX 0x1000
static inline int __load_deps_for_file (struct so_mem_desc* p,
					struct elfw(dyn)* iter,
					void* _data)
{
	void** data = (void**)_data;
	struct shl_list_node* search_dirs = (struct shl_list_node*)data[0];
	const char** failname = (const char**)data[1];
	struct shl_list_node* loaded_list = (struct shl_list_node*)data[2];

	if (iter->d_tag != DT_NEEDED)
		return CB_NEXT;

	/* Checks through search dirs by given filename in dynamic segment */
	struct shl_list_node* liter;
	struct libdir* liter_ent;
	shl_list_for_each_entry_auto (search_dirs, liter, liter_ent, list) {

		char filepath [FPATH_MAX] = {0};
		const char* filename = so_mem_strtab_off(p, iter->d_un.d_val);

		/* Build path */
		strncat (filepath, liter_ent->path, FPATH_MAX);
		strncat (filepath, "/", FPATH_MAX);
		strncat (filepath, filename, FPATH_MAX);

		int fd = sys_open (filepath, O_RDONLY);
		if (fd < 0)
			continue;
		/* If file exists, load it into memory,
		 *  build mem descriptor, then exit
		 */
		struct so_mem_desc* dep = malloc (sizeof(*dep));
		so_mem_init (dep);
		if (so_mem_mmap_fd (dep, p, fd) < EOK) {
			sys_close (fd);
			free (dep);
			*failname = filename;
			return -EINVAL;
		}
		shl_list_insert_safe (loaded_list, &(dep->loaded_list));
		sys_close (fd);
		/*
		if (so_mem_load_deps (dep, search_dirs, loaded_list, failname) < EOK)
			return -ENOMEM;
		*/
	}
	return EOK;
}

int so_mem_mmap_fd (struct so_mem_desc* dst,
		const struct so_mem_desc* p,
		int fd)
{
	char* load_max = NULL;
	char* load_min = NULL;
	if (__phdr_find_load_bounds_from_fd(
				fd,
				(void**)&load_min,
				(void**)&load_max) < EOK)
		return -EINVAL;
	size_t alloc_size = load_max - load_min;
	/* Map memory that could hold all load segments. */
	dst->base = sys_mmap (	NULL, alloc_size,
				PROT_NONE,
				MAP_PRIVATE | MAP_ANONYMOUS,
				-1, 0);
	if (dst->base == MAP_FAILED)
		return -ENOMEM;
	if (__phdr_mmap_load (dst, fd) < 0)
		return -ENOMEM;
	return EOK;
}

int so_mem_load_deps (	struct so_mem_desc* p,
			struct shl_list_node* search_dirs,
			struct shl_list_node* loaded_list,
			const char** failname)
{
	void * args [3] = {search_dirs, failname, loaded_list};
	if (so_mem_foreach_callback_dynamic (p, __load_deps_for_file, args) < 0)
		return -EINVAL;
	return EOK;
}