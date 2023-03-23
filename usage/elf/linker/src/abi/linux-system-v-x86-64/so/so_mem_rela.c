#include <abi/so_rela.h>

struct elfw(sym)* so_mem_get_symbol_by_name (
			struct so_mem_desc* p,
			const char* name)
{
	if (!p->symtab)
		return NULL;
	elfw(word) len = so_mem_symtab_len(p);
	for (elfw(word) i = 0; i < len; i++){
		struct elfw(sym)* iter = &p->symtab[i];
		/* Comparing strings, not hash (slow but simple) */
		const char* iter_str = so_mem_strtab_off (p, iter->st_name);
		if (strcmp(iter_str, name) == 0) {
			return iter;
		}
	}
	return NULL;
}