#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H

#include <abi/so.h>
#include <abi/page.h>

#include <abi/syscalls.h>

int so_mmap_fd (struct so_mem_desc* dst,
		const struct so_mem_desc* p,
		int fd);

int so_mem_load_deps (	struct so_mem_desc* p,
			struct shl_list_node* search_dirs,
			const char** failname);

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_LOAD_H */
