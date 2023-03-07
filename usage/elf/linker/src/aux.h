#ifndef __H_USAGE_ELF_LINKER_SRC_AUX_H
#define __H_USAGE_ELF_LINKER_SRC_AUX_H

#include <stdbool.h>

#include "../../../../shl_elf.h"

#define AT_NULL		0
#define AT_IGNORE	1
#define AT_EXECFD	2
#define AT_PHDR		3
#define AT_PHENT	4
#define AT_PHNUM	5
#define AT_PAGESZ	6
#define AT_BASE		7
#define AT_FLAGS	8
#define AT_ENTRY	9
#define AT_NOTELF	10
#define AT_UID		11
#define AT_EUID		12
#define AT_GID		13
#define AT_EGID		14

#define AT_NTYPES	AT_EGID+1

#define RETMNAME(macro) \
	case macro: return #macro

struct auxv
{
	int a_type;
	union {
		long a_val;
		void *a_ptr;
		void (*a_fnc)();
	} a_un;
};
__ASSERT(sizeof(struct auxv) == 16);

static inline const char* a_val_str (int a_type){
	switch (a_type){
		RETMNAME(AT_NULL);
		RETMNAME(AT_IGNORE);
		RETMNAME(AT_EXECFD);
		RETMNAME(AT_PHDR);
		RETMNAME(AT_PHENT);
		RETMNAME(AT_PHNUM);
		RETMNAME(AT_PAGESZ);
		RETMNAME(AT_BASE);
		RETMNAME(AT_FLAGS);
		RETMNAME(AT_ENTRY);
		RETMNAME(AT_NOTELF);
		RETMNAME(AT_UID);
		RETMNAME(AT_EUID);
		RETMNAME(AT_GID);
		RETMNAME(AT_EGID);
	}
	return "INVAL";
}

static inline bool is_valid_atype (struct auxv * p) {
	return p->a_type >= AT_NULL && p->a_type < AT_NTYPES;
}

#endif /* __H_USAGE_ELF_LINKER_SRC_AUX_H */
