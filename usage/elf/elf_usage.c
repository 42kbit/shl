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
#include <inttypes.h>
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
	void* elf_ehdr;
	/* General */
	void (*to_host_endian)(void* dst, void* src, int size);

	/* Header parsing */
	void (*elf_e_ident)	(struct elf_pinfo*, void* buf);
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

};

/* This macro generates getter functions like this one.
 *
 * static inline void elf32_e_phoff(struct elf_pinfo* pinfo, void* buf){
 * 	void* hdr = pinfo->elf_ehdr;
 * 	pinfo->to_host_endian(buf, &((struct elf32_ehdr*)hdr)->e_phoff,
 * 			sizeof(uint32_t));
 * }
*/
#define __gen_elf_hdr_getter(int, bits, element, element_size)				\
static inline void elf ## bits ## _ ## element(struct elf_pinfo* pinfo, void* buf) {	\
	void* hdr = pinfo->elf_ehdr;							\
	pinfo->to_host_endian(buf, &((struct elf ## bits ## _ehdr*)hdr)->element,	\
			sizeof(int ## element_size ## _t));				\
}

#define __gen_elf_hdr_getter_all_16(int, element)	\
	__gen_elf_hdr_getter(int, 32, element, 16)	\
	__gen_elf_hdr_getter(int, 64, element, 16)	

#define __gen_elf_hdr_getter_all(int, element)		\
	__gen_elf_hdr_getter(int, 32, element, 32)	\
	__gen_elf_hdr_getter(int, 64, element, 64)

__gen_elf_hdr_getter_all_16(uint, e_type);
__gen_elf_hdr_getter_all_16(uint, e_machine);
__gen_elf_hdr_getter_all(uint, e_version);
__gen_elf_hdr_getter_all(uint, e_entry);
__gen_elf_hdr_getter_all(uint, e_phoff);
__gen_elf_hdr_getter_all(uint, e_shoff);
__gen_elf_hdr_getter_all(uint, e_flags);
__gen_elf_hdr_getter_all_16(uint, e_ehsize);
__gen_elf_hdr_getter_all_16(uint, e_phentsize);
__gen_elf_hdr_getter_all_16(uint, e_phnum);
__gen_elf_hdr_getter_all_16(uint, e_shentsize);
__gen_elf_hdr_getter_all_16(uint, e_shnum);
__gen_elf_hdr_getter_all_16(uint, e_shstrndx); 	

static inline void elf_e_ident(struct elf_pinfo* pinfo, void* buf){
	memcpy(buf, pinfo->elf_ehdr, EI_NIDENT);
}

static inline int elf_pinfo_init_from_ehdr(struct elf_pinfo* info, void* mem){
	info->elf_ehdr = mem;
	switch (((struct elf32_ehdr*)mem)->e_ident[EI_CLASS]){
		case ELFCLASS32:
			info->elf_e_phoff	= elf32_e_phoff;
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
			break;
		case ELFCLASS64:
			info->elf_e_phoff	= elf64_e_phoff;
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

			break;
		case ELFCLASSNONE:
			return -EINVAL;
			break;
	}
	info->elf_e_ident = elf_e_ident;

	switch (((struct elf32_ehdr*)mem)->e_ident[EI_DATA]){
		case ELFDATA2LSB:
			info->to_host_endian = lsb_to_host;
			break;
		case ELFDATA2MSB:
			info->to_host_endian = msb_to_host;
			break;
	}
	return 0;
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
	fread (mem, fsz, fsz, file);
	fclose(file);
	
	struct elf_pinfo ops;
	if (elf_pinfo_init_from_ehdr(&ops, mem) < 0){
		fprintf(stderr, "pinfo init fail\n");
		free(mem);
		return -1;
	}
	uint64_t phoff = 0, type = 0, phentsize = 0;
	ops.elf_e_phoff(&ops, &phoff);
	ops.elf_e_type (&ops, &type);
	ops.elf_e_phentsize(&ops, &phentsize);
	printf("%lu, %p, %lu\n", phoff, (void*)type, phentsize);
	
	free(mem);
	return 0;
}