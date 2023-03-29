#include <abi/so_rela.h>

/* Does relocation to symbol depending on brela type */
static inline void __place_rela (struct so_mem_desc* base, struct elfw(rela)* brela,
				 struct so_mem_desc* src , struct elfw(sym) * srela)
{
	unsigned int reloc_type = ELF64_R_TYPE(brela->r_info);
	void * reldst = ptradd (base->base, brela->r_offset);
	void * symptr = ptradd (src->base , srela->st_value);

	switch (reloc_type) {
		case R_X86_64_GLOB_DAT:
		case R_X86_64_JUMP_SLOT:
		case R_X86_64_64:
		case R_X86_64_RELATIVE:
			*(void**)reldst = symptr;
			break;
		case R_X86_64_COPY:
			memcpy (reldst, symptr, srela->st_size);
			break;
	}
}

static inline int  __resolve_rela (
				struct so_mem_desc* p,
				struct shl_list_node* loaded_list,
				struct elfw(rela)* rela)
{
	struct elfw(sym)* sym = so_mem_get_symbol_by_rela (p, rela);
	const char* symname = so_mem_strtab_off(p, sym->st_name);

	struct shl_list_node* liter = NULL;
	struct so_mem_desc* liter_ent = NULL;
	shl_list_for_each_entry_auto (loaded_list, liter,
				      liter_ent, loaded_list)
	{
		struct elfw(sym)* found;
		found = so_mem_get_symbol_by_name (liter_ent, symname);
		if (!found)
			continue;
		__place_rela (p, rela, liter_ent, found);
		return EOK;
	}
	return -EINVAL;
}

struct elfw(sym)* so_mem_get_symbol_by_name (
			struct so_mem_desc* p,
			const char* name)
{
	if (!p->symtab)
		return NULL;
	elfw(word) len = so_mem_symtab_len(p);
	for (elfw(word) i = 0; i < len; i++){
		struct elfw(sym)* iter = &p->symtab[i];
		bool visible = ELF64_ST_BIND(iter->st_info) == STB_GLOBAL;
		bool valid_obj = ELF64_ST_TYPE(iter->st_info) == STT_OBJECT ||
				 ELF64_ST_TYPE(iter->st_info) == STT_FUNC;
		/* Comparing strings, not hash (slow but simple) */
		const char* iter_str = so_mem_strtab_off (p, iter->st_name);
		if (visible && valid_obj && strcmp(iter_str, name) == 0) {
			return iter;
		}
	}
	return NULL;
}

struct elfw(sym)* so_mem_get_symbol_by_rela (	struct so_mem_desc* p,
						struct elfw(rela)* rela)
{
	int idx = ELF64_R_SYM(rela->r_info);
	return &p->symtab[idx];
}

int so_mem_resolve_symbols (	struct so_mem_desc* p,
				struct shl_list_node* loaded_list)
{
	for (elfw(word) i = 0; i < p->rela_nent; i++) {
		struct elfw(rela)* iter = &p->rela[i];
		if (__resolve_rela (p, loaded_list, iter) < 0)
			return -EINVAL;
	}
	/* No lazy loading for now. */
	for (elfw(word) i = 0; i < p->rela_plt_nent; i++) {
		struct elfw(rela)* iter = &p->rela_plt[i];
		if (__resolve_rela (p, loaded_list, iter) < 0)
			return -EINVAL;
	}
	return EOK;
}

static inline void __die (){
	while (1);
}

int so_mem_init_rela (struct so_mem_desc* p) {
	//p->got [2] = __die;
	return EOK;
}