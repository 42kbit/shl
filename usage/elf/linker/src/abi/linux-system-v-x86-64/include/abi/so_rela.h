#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_RELA_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_RELA_H

#include <abi/so.h>

struct elfw(sym)* so_mem_get_symbol_by_rela (	struct so_mem_desc*,
						struct elfw(rela)* );
struct elfw(sym)* so_mem_get_symbol_by_name (struct so_mem_desc*, const char* );
int so_mem_resolve_symbols (struct so_mem_desc*, struct shl_list_node*);
int so_mem_init_rela (struct so_mem_desc*);

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_SO_RELA_H */
