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
	void	*elf_ehdr,
		*elf_phdr,
		*elf_shdr;

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
	
	/* Section Header parsing */
	void (*elf_sh_name)	(struct elf_pinfo*, void* buf);

};

/* This macro generates getter functions like this one.
 *
 * __gen_elf_getter(ehdr, uint, 32, e_type, 16)
 * 	Read as:
 * 		For 32 bit ehdr elf structure, create getter that writes to "buf" 16 bits of e_type field,
 * 		after converting to host endianness from endianness specified in e_ident[].
 * 
 * 	Results in:
 * 
 * static inline void elf32_e_type(struct elf_pinfo* pinfo, void* buf){
 * 	void* hdr = pinfo->elf_ehdr;
 * 	pinfo->to_host_endian(buf, &((struct elf32_ehdr*)hdr)->e_type,
 * 			sizeof(uint16_t));
 * }
*/

#define __gen_elf_getter(what, int, bits, element, element_size)				\
static inline void elf ## bits ## _ ## element(struct elf_pinfo* pinfo, void* buf) {		\
	void* hdr = pinfo->elf_ ## what;							\
	pinfo->to_host_endian(buf, &((struct elf ## bits ## _ ## what *)hdr)->element,		\
			sizeof(int ## element_size ## _t));					\
}

/* ELF header getters defines */

static inline void elf_e_ident(struct elf_pinfo* pinfo, void* buf){
	memcpy(buf, pinfo->elf_ehdr, EI_NIDENT);
}

__gen_elf_getter(ehdr, uint, 32, e_type, 16);
__gen_elf_getter(ehdr, uint, 64, e_type, 16);

__gen_elf_getter(ehdr, uint, 32, e_machine, 16);
__gen_elf_getter(ehdr, uint, 64, e_machine, 16);

__gen_elf_getter(ehdr, uint, 32, e_version, 32);
__gen_elf_getter(ehdr, uint, 64, e_version, 32);

__gen_elf_getter(ehdr, uint, 32, e_entry, 32);
__gen_elf_getter(ehdr, uint, 64, e_entry, 64);

__gen_elf_getter(ehdr, uint, 32, e_phoff, 32);
__gen_elf_getter(ehdr, uint, 64, e_phoff, 64);

__gen_elf_getter(ehdr, uint, 32, e_shoff, 32);
__gen_elf_getter(ehdr, uint, 64, e_shoff, 64);

__gen_elf_getter(ehdr, uint, 32, e_flags, 32);
__gen_elf_getter(ehdr, uint, 64, e_flags, 32);

__gen_elf_getter(ehdr, uint, 32, e_ehsize, 16);
__gen_elf_getter(ehdr, uint, 64, e_ehsize, 16);

__gen_elf_getter(ehdr, uint, 32, e_phentsize, 16);
__gen_elf_getter(ehdr, uint, 64, e_phentsize, 16);

__gen_elf_getter(ehdr, uint, 32, e_phnum, 16);
__gen_elf_getter(ehdr, uint, 64, e_phnum, 16);

__gen_elf_getter(ehdr, uint, 32, e_shentsize, 16);
__gen_elf_getter(ehdr, uint, 64, e_shentsize, 16);

__gen_elf_getter(ehdr, uint, 32, e_shnum, 16);
__gen_elf_getter(ehdr, uint, 64, e_shnum, 16);

__gen_elf_getter(ehdr, uint, 32, e_shstrndx, 16);
__gen_elf_getter(ehdr, uint, 64, e_shstrndx, 16);
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

	uint64_t shoff = 0, phoff = 0;
	info->elf_e_phoff(info, &phoff);
	info->elf_e_shoff(info, &shoff);
	info->elf_shdr = (char*)mem + shoff;
	info->elf_phdr = (char*)mem + phoff;

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
	uint64_t phoff = 0, type = 0, phentsize = 0, version = 0;
	ops.elf_e_phoff(&ops, &phoff);
	ops.elf_e_type (&ops, &type);
	ops.elf_e_phentsize(&ops, &phentsize);
	ops.elf_e_version(&ops, &version);
	printf("%lu, %p, %lu, %lu\n", phoff, (void*)type, phentsize, version);
	
	free(mem);
	return 0;
}