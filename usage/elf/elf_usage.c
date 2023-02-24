/* This should be a "simple" elf parser, that can read
 *	ELF Header
 *	ELF Segments Header
 *	ELF Segments
 *	ELF Section Header
 *	ELF Sections
 * using shl_elf.h file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include "../../shl_elf.h"

#undef  LITTLE_ENDIAN
#undef  BIG_ENDIAN   
#undef  PDP_ENDIAN   
#undef  LITTLE_ENDIAN_ORDER
#undef  BIG_ENDIAN_ORDER
#undef  PDP_ENDIAN_ORDER
#undef  ENDIAN_ORDER 

/* 
 * Danke,
 * https://stackoverflow.com/a/9283155/16209002
 * This part of code determines ENDIANNESS of host machine.
 * Will be used when converting byte order stuff, known from e_ident[EI_DATA]
*/
#define LITTLE_ENDIAN_ORDER 	0x41424344UL 
#define BIG_ENDIAN_ORDER    	0x44434241UL
#define PDP_ENDIAN_ORDER    	0x42414443UL
#define ENDIAN_ORDER 		('ABCD') 

#if ENDIAN_ORDER==LITTLE_ENDIAN_ORDER
	#define LITTLE_ENDIAN
#elif ENDIAN_ORDER==BIG_ENDIAN_ORDER
	#define BIG_ENDIAN
	#error "BIG_ENDIAN is not supported yet."
#elif ENDIAN_ORDER==PDP_ENDIAN_ORDER
	#define PDP_ENDIAN
	#error "PDP_ENDIAN is not supported."
#else
	#error "Unknown endianness on host machine."
#endif

/* bit(0) = 0b00000001 */
/* bit(4) = 0b00010000 */
/* bit(5) = 0b00100000 */
/* bit(7) = 0b10000000 */

#define bit(x) (1<<x)

/* bitmask(0) = 0b00000000 */
/* bitmask(4) = 0b00001111 */
/* bitmask(5) = 0b00011111 */
/* bitmask(7) = 0b01111111 */

#define bitmask(x) (bit(x) - 1)

#define bitoff(x, by) (x << by)
#define bitset(x, mask) (x & mask)

#define bitclr(x, mask) (x & ~mask)

/* in "val", from "where" bit, take "by" */

/* val = 1100 0110 
 * bitcut(val, 0, 4) = 0000 0110
 * bitcut(val, 4, 4) = 0000 1100
 * */

#define bitcut(val, where, by) ((val >> where) & bitmask(by))

#define RETMNAME(macro) \
	case macro: return #macro

#define STRINVAL "UNKNOWN"

static inline void reverse_bytes(void* _dst, void* _src, int size){
	for (char* iter_src = (char*)_src, *iter_dst = ((char*)_dst) + size-1;
		size > 0;
		size--, iter_src++, iter_dst--)
	{
		*iter_dst = *iter_src;
	}
}

static inline void lsb_to_host(void* dst, void* src, int size){
#ifdef BIG_ENDIAN
	return reverse_bytes(dst, src, size);
#else
	memcpy(dst, src, size);
#endif
}
static inline void msb_to_host(void* dst, void* src, int size){
#ifdef LITTLE_ENDIAN
	reverse_bytes(dst, src, size);
#else
	memcpy(dst, src, size);
#endif
}

/* ELF Parsing info 
 * Holds data and functions, used in parsing ELF file,
 * Used to abstract out things such as:
 * 	Endianness
 * 	Byte Order
 * In order to append functionality, a elf-bullshit agnostic
 * function prototype must be made, such as ones that you see below.
 */
struct elf_pinfo {
	void	*elf_ehdr,
		*elf_phdr,
		*elf_shdr,
		*elf_shstr;

	int	shdr_size,
		phdr_size;

	/* General */
	void (*to_host_endian)(void* dst, void* src, int size);

	/* Header parsing */
	void (*elf_e_type)	(struct elf_pinfo*, void* buf);	 
	void (*elf_e_machine)	(struct elf_pinfo*, void* buf);
	void (*elf_e_version)	(struct elf_pinfo*, void* buf);
	void (*elf_e_entry)	(struct elf_pinfo*, void* buf);
	void (*elf_e_phoff)	(struct elf_pinfo*, void* buf);
	void (*elf_e_shoff)	(struct elf_pinfo*, void* buf);
	void (*elf_e_flags)	(struct elf_pinfo*, void* buf);
	void (*elf_e_ehsize)	(struct elf_pinfo*, void* buf);
	void (*elf_e_phentsize)	(struct elf_pinfo*, void* buf);
	void (*elf_e_phnum)	(struct elf_pinfo*, void* buf);
	void (*elf_e_shentsize)	(struct elf_pinfo*, void* buf);
	void (*elf_e_shnum)	(struct elf_pinfo*, void* buf);
	void (*elf_e_shstrndx)	(struct elf_pinfo*, void* buf);
	
	/* Section Header parsing */
	void (*elf_sh_name)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_type)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_flags)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_addr)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_offset)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_size)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_link)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_info)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_addralign)(struct elf_pinfo*, int idx, void* buf);
	void (*elf_sh_entsize)	(struct elf_pinfo*, int idx, void* buf);

	void (*elf_p_type)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_p_offset)	(struct elf_pinfo*, int idx, void* buf);	
	void (*elf_p_vaddr)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_p_paddr)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_p_filesz)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_p_memsz)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_p_flags)	(struct elf_pinfo*, int idx, void* buf);
	void (*elf_p_align)	(struct elf_pinfo*, int idx, void* buf);
};

/* This macro generates getter functions like this one.
 *
 * __gen_elf_getter(ehdr, 32, e_type)
 * 	Read as:
 * 		For 32 bit ehdr elf structure, create getter that writes to "buf" 16 bits of e_type field,
 * 		after converting to host endianness from endianness specified in e_ident[].
 * 
 * 	Results in:
 * 
 * static inline void elf32_e_type(struct elf_pinfo* pinfo, void* buf){
 * 	void* hdr = pinfo->elf_ehdr;
 * 	pinfo->to_host_endian(buf, &((struct elf32_ehdr*)hdr)->e_type,
 * 			sizeof(((struct elf32_ehdr*)hdr)->e_type));
 * }
*/

#define __gen_elf_getter(what, bits, element)							\
static inline void elf ## bits ## _ ## element(struct elf_pinfo* pinfo, void* buf) {		\
	void* hdr = pinfo->elf_ ## what;							\
	pinfo->to_host_endian(buf, &((struct elf ## bits ## _ ## what *)hdr)->element,		\
			sizeof(((struct elf ## bits ## _ ## what *)hdr)->element));		\
}

#define __gen_elf_getter_32_64(what, element)	\
	__gen_elf_getter(what, 32, element)	\
	__gen_elf_getter(what, 64, element)	

/* Same thing but with offsets, for arrays.
 * In order for that to work, the "what ## _size" field in "elf_pinfo" is required.
 * For example: if generating shdr - shdr_size is needed.
 * 
 * Ofc size can be hardcoded, but i'll let it be like this for now.
*/
#define __gen_elf_getter_off(what, bits, element)						\
static inline void elf ## bits ## _ ## element(struct elf_pinfo* pinfo, int idx, void* buf) {	\
	const int hsize = pinfo->what ## _size;							\
	void* hdr = ((char*)pinfo->elf_ ## what) + hsize * idx;					\
	pinfo->to_host_endian(buf, &((struct elf ## bits ## _ ## what *)hdr)->element,		\
			sizeof(((struct elf ## bits ## _ ## what *)hdr)->element));		\
}

#define __gen_elf_getter_off_32_64(what, element)	\
	__gen_elf_getter_off(what, 32, element)		\
	__gen_elf_getter_off(what, 64, element)	

/* ELF header getters defines */

static inline void elf_e_ident(struct elf_pinfo* pinfo, void* buf){
	memcpy(buf, pinfo->elf_ehdr, EI_NIDENT);
}

__gen_elf_getter_32_64(ehdr, e_type);
__gen_elf_getter_32_64(ehdr, e_machine);
__gen_elf_getter_32_64(ehdr, e_version);
__gen_elf_getter_32_64(ehdr, e_entry);
__gen_elf_getter_32_64(ehdr, e_phoff);
__gen_elf_getter_32_64(ehdr, e_shoff);
__gen_elf_getter_32_64(ehdr, e_flags);
__gen_elf_getter_32_64(ehdr, e_ehsize);
__gen_elf_getter_32_64(ehdr, e_phentsize);
__gen_elf_getter_32_64(ehdr, e_phnum);
__gen_elf_getter_32_64(ehdr, e_shentsize);
__gen_elf_getter_32_64(ehdr, e_shnum);
__gen_elf_getter_32_64(ehdr, e_shstrndx);

/* ELF Sections getters defines 
 * Those one are served with offset param, for further info check
 * __gen_elf_getter_off macro, that generates getters.
*/

__gen_elf_getter_off_32_64(shdr, sh_name);
__gen_elf_getter_off_32_64(shdr, sh_type);
__gen_elf_getter_off_32_64(shdr, sh_flags);
__gen_elf_getter_off_32_64(shdr, sh_addr);
__gen_elf_getter_off_32_64(shdr, sh_offset);
__gen_elf_getter_off_32_64(shdr, sh_size);
__gen_elf_getter_off_32_64(shdr, sh_link);
__gen_elf_getter_off_32_64(shdr, sh_info);
__gen_elf_getter_off_32_64(shdr, sh_addralign);
__gen_elf_getter_off_32_64(shdr, sh_entsize);

/* ELF Segments getters defines */

__gen_elf_getter_off_32_64(phdr, p_type);	
__gen_elf_getter_off_32_64(phdr, p_offset);
__gen_elf_getter_off_32_64(phdr, p_vaddr);
__gen_elf_getter_off_32_64(phdr, p_paddr);
__gen_elf_getter_off_32_64(phdr, p_filesz);
__gen_elf_getter_off_32_64(phdr, p_memsz); 
__gen_elf_getter_off_32_64(phdr, p_flags);
__gen_elf_getter_off_32_64(phdr, p_align);

static inline int elf_pinfo_init_from_ehdr(struct elf_pinfo* info, void* mem){
	info->elf_ehdr = mem;
	switch (((struct elf32_ehdr*)mem)->e_ident[EI_CLASS]){
		case ELFCLASS32:
                        info->elf_e_type	= elf32_e_type;
                        info->elf_e_machine	= elf32_e_machine;
                        info->elf_e_version	= elf32_e_version;
                        info->elf_e_entry	= elf32_e_entry;
                        info->elf_e_phoff	= elf32_e_phoff;
                        info->elf_e_shoff	= elf32_e_shoff;
                        info->elf_e_flags	= elf32_e_flags;
                        info->elf_e_ehsize	= elf32_e_ehsize;
                        info->elf_e_phentsize	= elf32_e_phentsize;
                        info->elf_e_phnum	= elf32_e_phnum;
                        info->elf_e_shentsize	= elf32_e_shentsize;
                        info->elf_e_shnum	= elf32_e_shnum;
                        info->elf_e_shstrndx	= elf32_e_shstrndx;
			
			info->elf_sh_name 	= elf32_sh_name;
                        info->elf_sh_type	= elf32_sh_type;
                        info->elf_sh_flags	= elf32_sh_flags;
                        info->elf_sh_addr	= elf32_sh_addr;
                        info->elf_sh_offset	= elf32_sh_offset;
                        info->elf_sh_size	= elf32_sh_size;
                        info->elf_sh_link	= elf32_sh_link;
                        info->elf_sh_info	= elf32_sh_info;
                        info->elf_sh_addralign	= elf32_sh_addralign;
                        info->elf_sh_entsize	= elf32_sh_entsize;

			info->elf_p_type	= elf32_p_type;
                        info->elf_p_offset	= elf32_p_offset;
                        info->elf_p_vaddr	= elf32_p_vaddr;
                        info->elf_p_paddr	= elf32_p_paddr;
                        info->elf_p_filesz	= elf32_p_filesz;
                        info->elf_p_memsz	= elf32_p_memsz;
                        info->elf_p_flags	= elf32_p_flags;
                        info->elf_p_align	= elf32_p_align;
			break;
		case ELFCLASS64:
                        info->elf_e_type	= elf64_e_type;
                        info->elf_e_machine	= elf64_e_machine;
                        info->elf_e_version	= elf64_e_version;
                        info->elf_e_entry	= elf64_e_entry;
                        info->elf_e_phoff	= elf64_e_phoff;
                        info->elf_e_shoff	= elf64_e_shoff;
                        info->elf_e_flags	= elf64_e_flags;
                        info->elf_e_ehsize	= elf64_e_ehsize;
                        info->elf_e_phentsize	= elf64_e_phentsize;
                        info->elf_e_phnum	= elf64_e_phnum;
                        info->elf_e_shentsize	= elf64_e_shentsize;
                        info->elf_e_shnum	= elf64_e_shnum;
                        info->elf_e_shstrndx	= elf64_e_shstrndx;

			info->elf_sh_name 	= elf64_sh_name;
                        info->elf_sh_type	= elf64_sh_type;
                        info->elf_sh_flags	= elf64_sh_flags;
                        info->elf_sh_addr	= elf64_sh_addr;
                        info->elf_sh_offset	= elf64_sh_offset;
                        info->elf_sh_size	= elf64_sh_size;
                        info->elf_sh_link	= elf64_sh_link;
                        info->elf_sh_info	= elf64_sh_info;
                        info->elf_sh_addralign	= elf64_sh_addralign;
                        info->elf_sh_entsize	= elf64_sh_entsize;

			info->elf_p_type	= elf64_p_type;
                        info->elf_p_offset	= elf64_p_offset;
                        info->elf_p_vaddr	= elf64_p_vaddr;
                        info->elf_p_paddr	= elf64_p_paddr;
                        info->elf_p_filesz	= elf64_p_filesz;
                        info->elf_p_memsz	= elf64_p_memsz;
                        info->elf_p_flags	= elf64_p_flags;
                        info->elf_p_align	= elf64_p_align;
			break;
		case ELFCLASSNONE:
		default:
			return -EINVAL;
	}

	switch (((struct elf32_ehdr*)mem)->e_ident[EI_DATA]){
		case ELFDATA2LSB:
			info->to_host_endian = lsb_to_host;
			break;
		case ELFDATA2MSB:
			info->to_host_endian = msb_to_host;
			break;
		default:
			return -EINVAL;
	}

	uint64_t shoff = 0, phoff = 0;
	info->elf_e_phoff(info, &phoff);
	info->elf_e_shoff(info, &shoff);
	info->elf_shdr = (char*)mem + shoff;
	info->elf_phdr = (char*)mem + phoff;
	
	info->phdr_size = info->shdr_size = 0;

	info->elf_e_phentsize(info, &info->phdr_size);
	info->elf_e_shentsize(info, &info->shdr_size);
	/* get address of string table */
	
	uint64_t shstrndx = 0;
	void* shstrndx_off = NULL;

	info->elf_e_shstrndx(info, &shstrndx);
	info->elf_sh_offset(info, shstrndx, &shstrndx_off);
	
	void* shstrndx_addr = (void*)((unsigned long)mem + (unsigned long)shstrndx_off);
	info->elf_shstr = shstrndx_addr;

	return 0;
}

static inline const char* elf_shstr_off(struct elf_pinfo* pinfo, int offset){
	return (const char*)pinfo->elf_shstr + offset;
}

static inline const char* elf_ehdr_strclass(uint64_t class){
	switch (class){
		RETMNAME(ELFCLASS32);
		RETMNAME(ELFCLASS64);
		RETMNAME(ELFCLASSNONE);
		default: return STRINVAL;
	}
}

static inline const char* elf_ehdr_strdata(uint64_t data){
	switch (data){
		RETMNAME(ELFDATANONE);
		RETMNAME(ELFDATA2LSB);
		RETMNAME(ELFDATA2MSB);
		default: return STRINVAL;
	}
}

static inline const char* elf_ehdr_strversion(uint64_t version){
	switch (version){
		RETMNAME(EV_NONE);
		RETMNAME(EV_CURRENT);
		default: return STRINVAL;
	}
}

static inline const char* elf_ehdr_strtype(uint64_t type){
	switch (type){
		RETMNAME(ET_NONE);
		RETMNAME(ET_REL );
		RETMNAME(ET_EXEC);
		RETMNAME(ET_DYN	);
		RETMNAME(ET_CORE);
		RETMNAME(ET_NUM	);
		default: return STRINVAL;
	}
}

static inline const char* elf_phdr_strtype(uint64_t type){
	switch (type){
		RETMNAME(PT_NULL);
		RETMNAME(PT_LOAD);
		RETMNAME(PT_DYNAMIC);
		RETMNAME(PT_INTERP);
		RETMNAME(PT_NOTE);
		RETMNAME(PT_SHLIB);
		RETMNAME(PT_PHDR);
		RETMNAME(PT_TLS);
		RETMNAME(PT_NUM);
		default: return STRINVAL;
	}
}

static inline const char* elf_shdr_strtype(uint64_t type){
	switch (type){
		RETMNAME(SHT_NULL); 
		RETMNAME(SHT_PROGBITS);
        	RETMNAME(SHT_SYMTAB);
        	RETMNAME(SHT_STRTAB);
        	RETMNAME(SHT_RELA);
        	RETMNAME(SHT_HASH);
		RETMNAME(SHT_DYNAMIC);
        	RETMNAME(SHT_NOTE);
		RETMNAME(SHT_NOBITS);
        	RETMNAME(SHT_REL); 
		RETMNAME(SHT_SHLIB);
        	RETMNAME(SHT_DYNSYM);
        	RETMNAME(SHT_INIT_ARRAY);
        	RETMNAME(SHT_FINI_ARRAY);
        	RETMNAME(SHT_PREINIT_ARRAY);
        	RETMNAME(SHT_GROUP);
        	RETMNAME(SHT_SYMTAB_SHNDX);
        	RETMNAME(SHT_NUM); 
		default: return STRINVAL;
	}
}

static inline void elf_print_ehdr (struct elf_pinfo* pinfo){
	unsigned char ident[EI_NIDENT];
	elf_e_ident(pinfo, ident);
	uint64_t class 		= ident[EI_CLASS],
		 data  		= ident[EI_DATA],
		 version	= ident[EI_VERSION],
		 osabi		= ident[EI_OSABI],
		 abi_ver	= ident[EI_ABIVERSION],
		 type = 0, machine = 0, vversion = 0,
		 entry = 0, phoff = 0, shoff = 0,
		 flags = 0, ehsize = 0, phentsize = 0,
		 phnum = 0, shentsize = 0, shnum = 0,
		 shstrndx = 0;

	pinfo->elf_e_type	(pinfo, &type);
	pinfo->elf_e_machine	(pinfo, &machine);
	pinfo->elf_e_version	(pinfo, &vversion);
	pinfo->elf_e_entry	(pinfo, &entry);
	pinfo->elf_e_phoff	(pinfo, &phoff);
	pinfo->elf_e_shoff	(pinfo, &shoff);
	pinfo->elf_e_flags	(pinfo, &flags);
	pinfo->elf_e_ehsize	(pinfo, &ehsize);
	pinfo->elf_e_phentsize	(pinfo, &phentsize);
	pinfo->elf_e_phnum	(pinfo, &phnum);
	pinfo->elf_e_shentsize	(pinfo, &shentsize);
	pinfo->elf_e_shnum	(pinfo, &shnum);
	pinfo->elf_e_shstrndx	(pinfo, &shstrndx);

	printf(" e_ident[]:     %s%c%c%c\n",
		ident[EI_MAG0] == ELFMAG0? "'0x7f'" : "???",
		ident[EI_MAG1],
		ident[EI_MAG2],
		ident[EI_MAG3]
	); 
	printf("  EI_CLASS 	%s\n", 		elf_ehdr_strclass(class)); 
	printf("  EI_DATA  	%s\n", 		elf_ehdr_strdata(data)); 
	printf("  EI_VERSION  	%s\n", 		elf_ehdr_strversion(version));
	printf("  EI_OSABI  	0x%0lx\n", 	osabi); 
	printf("  EI_ABIVERSION	0x%0lx\n", 	abi_ver); 

	printf(" e_type		%s\n", 		elf_ehdr_strtype(type));
	printf(" e_machine	0x%0lx\n", 	machine);
	printf(" e_version	%s\n", 		elf_ehdr_strversion(vversion));
	printf(" e_entry	0x%0lx\n", 	entry);
	printf(" e_phoff	%ld\n", 	phoff);
	printf(" e_shoff	%ld\n", 	shoff);
	printf(" e_flags	0x%0lx\n",	flags);
	printf(" e_ehsize	%ld\n",		ehsize);
	printf(" e_phentsize	%ld\n",		phentsize);
	printf(" e_phnum	%ld\n",		phnum);
	printf(" e_shentsize	%ld\n",		shentsize);
	printf(" e_shnum	%ld\n",		shnum);
	printf(" e_shstrndx	%ld\n",		shstrndx);
}

static inline void elf_print_phdr (struct elf_pinfo* pinfo, int idx){
	uint64_t type = 0, offset = 0,
        	 vaddr = 0, paddr = 0,
        	 filesz = 0, memsz = 0,
		 flags = 0, align = 0;
	pinfo->elf_p_type	(pinfo, idx, &type);
        pinfo->elf_p_offset	(pinfo, idx, &offset);
        pinfo->elf_p_vaddr	(pinfo, idx, &vaddr);
        pinfo->elf_p_paddr	(pinfo, idx, &paddr);
        pinfo->elf_p_filesz	(pinfo, idx, &filesz);
        pinfo->elf_p_memsz	(pinfo, idx, &memsz);
        pinfo->elf_p_flags	(pinfo, idx, &flags);
        pinfo->elf_p_align	(pinfo, idx, &align);
	printf("  p_type	%s\n", elf_shdr_strtype(type));
	printf("  p_offset	%ld\n", offset);
	printf("  p_vaddr	0x%0lx\n", vaddr);
	printf("  p_paddr	0x%0lx\n", paddr);
	printf("  p_filesz	%ld\n", filesz);
	printf("  p_memsz	%ld\n", memsz);
	printf("  p_flags	");
	if (bitset(flags, PF_R)){
		flags = bitclr(flags, PF_R);
		printf("R");
	}
	if (bitset(flags, PF_W)){
		flags = bitclr(flags, PF_W);
		printf("W");
	}
	if (bitset(flags, PF_X)){
		flags = bitclr(flags, PF_X);
		printf("X");
	}
	if (bitset(flags, PF_MASKPROC)){
		flags = bitclr(flags, PF_MASKPROC);
		printf(" MASKPROC");
	}
	printf("\n");
	printf("  p_align	0x%0lx\n", align);
}

static inline void elf_print_shdr (struct elf_pinfo* pinfo, int idx){
	uint64_t name = 0, type = 0,
		 flags = 0, addr = 0,
		 offset = 0, size = 0,
		 link = 0, info = 0,
		 addralign = 0, entsize = 0;

	pinfo->elf_sh_name		(pinfo, idx, &name);
	pinfo->elf_sh_type		(pinfo, idx, &type);
        pinfo->elf_sh_flags		(pinfo, idx, &flags);
	pinfo->elf_sh_addr		(pinfo, idx, &addr);
        pinfo->elf_sh_offset		(pinfo, idx, &offset);
        pinfo->elf_sh_size		(pinfo, idx, &size);
        pinfo->elf_sh_link		(pinfo, idx, &link);
        pinfo->elf_sh_info		(pinfo, idx, &info);
	pinfo->elf_sh_addralign		(pinfo, idx, &addralign);
        pinfo->elf_sh_entsize		(pinfo, idx, &entsize);

	printf("  sh_name		%s\n", elf_shstr_off(pinfo, name));
	printf("  sh_type		%s\n", elf_shdr_strtype(type));

	printf("  sh_flags		");
	if (bitset(flags, SHF_WRITE)){
		flags = bitclr(flags, SHF_WRITE);
		printf("W");
	}
	if (bitset(flags, SHF_ALLOC)){
		flags = bitclr(flags, SHF_ALLOC);
		printf("A");
	}
	if (bitset(flags, SHF_EXECINSTR)){
		flags = bitclr(flags, SHF_EXECINSTR);
		printf("E");
	}
	printf("\n");
	printf("  sh_addr		0x%0lx\n", 	addr);
	printf("  sh_offset		%ld\n", 	offset);
	printf("  sh_size		%ld\n",		size);
	printf("  sh_link		0x%0lx\n", 	link);
	printf("  sh_info		0x%0lx\n", 	info);
	printf("  sh_addralign		0x%0lx\n", 	addralign);
	printf("  sh_entsize		%ld\n", 	entsize);
}

int main(int argc, char* argv[]){
	if (argc < 2){
		printf("Usage: elf_usage <filename>\n");
		return 0;
	}
	FILE* file;
	long fsz;
	file = fopen(argv[1], "r");
	if (!file){
		fprintf(stderr, "cannot open file, perhaps invalid name.\n");
		return -1;
	}
	
	fseek(file, 0L, SEEK_END);
	fsz = ftell(file);
	rewind(file);
	void* mem = malloc(fsz);
	if (!mem){
		fprintf(stderr, "malloc failed, perhaps file is too big.\n");
		fclose(file);
		return -1;
	}
	fread (mem, fsz, 1, file);
	fclose(file);
	
	struct elf_pinfo ops;
	if (elf_pinfo_init_from_ehdr(&ops, mem) < 0){
		fprintf(stderr, "pinfo init fail\n");
		free(mem);
		return -1;
	}

	int phnum = 0, shnum = 0;
	ops.elf_e_phnum(&ops, &phnum);
	ops.elf_e_shnum(&ops, &shnum);

	printf("ELF HEADER\n");
	elf_print_ehdr(&ops);
	for (int i = 0; i < phnum; i++){
		printf("ELF PROGRAM HEADER %d\n", i);
		elf_print_phdr(&ops, i);
	}
	for (int i = 0; i < shnum; i++){
		printf("ELF SECTION HEADER %d\n", i);
		elf_print_shdr(&ops, i);
	}
	
	free(mem);
	return 0;
}