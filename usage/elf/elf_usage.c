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

static inline uint16_t u16_reverse(uint16_t n){
	unsigned char *np = (unsigned char *)&n;
	return 	((uint16_t)np[0] << 8)  |
		 (uint16_t)np[1];
}
static inline uint32_t u32_reverse(uint32_t n){
	unsigned char *np = (unsigned char *)&n;
	return 	((uint32_t)np[0] << 24) |
		((uint32_t)np[1] << 16) |
		((uint32_t)np[2] << 8)  |
		 (uint32_t)np[3];
}
static inline uint64_t u64_reverse(uint64_t n){
	unsigned char *np = (unsigned char *)&n;
	return 	
		((uint64_t)np[0] << 56) |
		((uint64_t)np[1] << 48) |
		((uint64_t)np[2] << 40) |
		((uint64_t)np[3] << 32) |
		((uint64_t)np[4] << 24) |
		((uint64_t)np[5] << 16) |
		((uint64_t)np[6] << 8)  |
		 (uint64_t)np[7];
}
static inline uint16_t u16_msb_to_host(uint16_t n){
#ifdef LITTLE_ENDIAN
	return u16_reverse(n);
#else
	return n;
#endif
}
static inline uint32_t u32_msb_to_host(uint32_t n){
#ifdef LITTLE_ENDIAN
	return u32_reverse(n);
#else
	return n;
#endif
}
static inline uint64_t u64_msb_to_host(uint64_t n){
#ifdef LITTLE_ENDIAN
	return u64_reverse(n);
#else
	return n;
#endif
}
static inline uint16_t u16_lsb_to_host(uint16_t n){
#ifdef BIG_ENDIAN
	return u16_reverse(n);
#else
	return n;
#endif
}
static inline uint32_t u32_lsb_to_host(uint32_t n){
#ifdef BIG_ENDIAN
	return u32_reverse(n);
#else
	return n;
#endif
}
static inline uint64_t u64_lsb_to_host(uint64_t n){
#ifdef BIG_ENDIAN
	return u64_reverse(n);
#else
	return n;
#endif
}

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
	/* General */
	void (*to_host_endian)(void* dst, void* src, int size);

	/* Header parsing */
	void (*elf_phoff)(struct elf_pinfo*, void* elf, void* buf);
};


static inline void elf32_phoff(struct elf_pinfo* pinfo, void* hdr, void* buf){
	pinfo->to_host_endian(buf, &((struct elf32_ehdr*)hdr)->e_phoff,
			sizeof(uint32_t));
}

static inline void elf64_phoff(struct elf_pinfo* pinfo, void* hdr, void* buf){
	pinfo->to_host_endian(buf, &((struct elf64_ehdr*)hdr)->e_phoff,
			sizeof(uint64_t));
}

static inline int elf_pinfo_init_from_ehdr(struct elf_pinfo* info, void* mem){
	switch (((struct elf32_ehdr*)mem)->e_ident[EI_CLASS]){
		case ELFCLASS32:
			info->elf_phoff = elf32_phoff;
			break;
		case ELFCLASS64:
			info->elf_phoff = elf64_phoff;
			break;
		case ELFCLASSNONE:
			return -EINVAL;
			break;
	}

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
	uint64_t phoff;
	ops.elf_phoff(&ops, mem, &phoff);
	printf("%lu\n", phoff);
	
	free(mem);
	return 0;
}