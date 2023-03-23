#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_H

#include <shl_elf.h>
#include <shl_list.h>

#include <lib/string.h>
#include <lib/io.h>
#include <stdbool.h>

#include <errno.h>

#include <abi/libdir.h>

#include <abi/syscalls.h>

/* Descriptor of a loaded in memory shared object. */
#define PHDR_MAX_LOAD 32
#define DYN_MAX_DEPS 32
struct so_mem_desc {
	void	*base;
	void	**got;
	struct elfw(dyn)  *dynamic;
	
	/* System V Hash (Not GNU) */
	elfw(word)* hash;

	struct elfw(rela) *rela;
	elfw(word) rela_nent;

	struct elfw(rela) *rela_plt;
	elfw(word) rela_plt_nent;
	
	struct elfw(sym)* symtab;
	const char* strtab;
	
	struct shl_list_node loaded_list;
};

const char* so_mem_strtab_off (struct so_mem_desc* p, unsigned int offset);
elfw(word) so_mem_symtab_len (struct so_mem_desc* p);

#define CB_NEXT EOK
int so_mem_foreach_callback_dynamic (
			struct so_mem_desc* p,
			int (*callback) (
				struct so_mem_desc* so,
				struct elfw(dyn)* current,
				void* data
			),
			void* data);

void so_mem_init (struct so_mem_desc* p);

int so_mem_init_dynamic (struct so_mem_desc* p,
			 struct elfw(dyn)* dynamic);

int so_mem_init_desc_from_phdr (
			struct so_mem_desc* p,
			struct elfw(phdr)* phdr,
			elfw(half) phnum);

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_H */
