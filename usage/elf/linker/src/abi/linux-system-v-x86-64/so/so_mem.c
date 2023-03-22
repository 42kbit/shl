/* This file describes functions for memory descriptor
*/
#include <abi/so.h>

const char* so_mem_strtab_off (
			struct so_mem_desc* p,
			unsigned int offset)
{
	return p->strtbl + offset;
}

int so_mem_foreach_callback_dynamic (
			struct so_mem_desc* p,
			int (*callback) (
				struct so_mem_desc* so,
				struct elfw(dyn)* current,
				void* data
			),
			void* data)
{
	int retval = EOK;
	for (int i = 0; p->dynamic[i].d_tag != DT_NULL; i++)
		if ((retval = callback (p, &p->dynamic[i], data)) != CB_NEXT)
			break;
	return retval;
}

int so_mem_init_dynamic (
			struct so_mem_desc* p,
			struct elfw(dyn)* dynamic)
{
	p->dynamic = dynamic;

	elfw(word) rela_entsz = 0;
	elfw(word) rela_tblsz = 0;

	/* Now, parse dynamic segment */
	struct elfw(dyn)* dyntbl = p->dynamic;
	for (int i = 0; dyntbl[i].d_tag != DT_NULL; i++) {

		struct elfw(dyn)* iter = &dyntbl[i];

		switch (iter->d_tag){
		case DT_STRTAB:
			p->strtbl = ptradd (p->base, iter->d_un.d_ptr);
			continue;
		case DT_PLTGOT:
			p->got = ptradd(p->base, iter->d_un.d_ptr);
			continue;
		case DT_RELA:
			p->rela = ptradd(p->base, iter->d_un.d_ptr);
			continue;
		case DT_RELAENT:
			rela_entsz = iter->d_un.d_val;
			continue;
		case DT_RELASZ:
			rela_tblsz = iter->d_un.d_val;
			continue;
		}
	}
	if (rela_entsz != 0)
		p->rela_nent = rela_tblsz / rela_entsz;

	return EOK;
}

int so_mem_init_desc_from_phdr (
			struct so_mem_desc* p,
			struct elfw(phdr)* phdr,
			elfw(half) phnum)
{
	memzero (p);

	/* The pointer itself is valid, cuz it points into ELF ehdr,
	 * but content of header itself is undefined
	 * (no word about it in System V ABI),
	 * so p->base is valid(?) for ELF version 1.
	*/
	p->base = (char*)phdr - sizeof(struct elfw(ehdr));
	void* dynamic = NULL;

	for (int i = 0; i < phnum; i++) {
		struct elfw(phdr)* iter = &phdr[i];
		switch(iter->p_type){
		case PT_DYNAMIC:
			dynamic = ptradd(p->base, iter->p_vaddr);
			continue;
		}
	}
	
	if (!dynamic)
		return -EINVAL;
	so_mem_init_dynamic (p, dynamic);
	return EOK;
}