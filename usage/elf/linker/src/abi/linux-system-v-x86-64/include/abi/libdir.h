#ifndef __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_LIBDIR_H
#define __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_LIBDIR_H

struct libdir{
	const char* path;
	struct shl_list_node list;
};

static inline void libdir_add_search_path (
	struct shl_list_node* head,
	struct shl_list_node* new_node, /* New allocated node */
	const char* path)
{
	struct libdir* entry = container_of(new_node, struct libdir, list);
	entry->path = path;
	shl_list_init_node (new_node);
	shl_list_insert_safe (head, new_node);
}

#endif /* __H_USAGE_ELF_LINKER_SRC_ABI_LINUX_SYSTEM_V_X86_64_INCLUDE_ABI_LIBDIR_H */
