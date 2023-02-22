/* Source:
 * 	https://refspecs.linuxfoundation.org/elf/elf.pdf 
*/
#ifndef __H_SHL_ELF_H
#define __H_SHL_ELF_H

/* 
 * Compile time sanity check
 * Usage:
 * __ASSERT(sizeof(int) == 4);
 * Will drop compile error if assert fails.
 */
#define GLUE(a,b) __GLUE(a,b)
#define __GLUE(a,b) a ## b
#define CVERIFY(expr, msg) typedef char GLUE (compiler_verify_, msg) [(expr) ? (+1) : (-1)]
#define __ASSERT(exp) CVERIFY (exp, __LINE__)

#ifndef NULL
#define NULL ((void*)0)
#endif

#define container_of(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

#define shl_get_entry(ptr, type, name) \
	container_of(ptr, type, name)

__ASSERT(sizeof(int) == 4);
__ASSERT(sizeof(short int) == 2);
__ASSERT(sizeof(char) == 1);

typedef unsigned int	Elf32_Addr;
typedef unsigned int	Elf32_Off;
typedef unsigned int	Elf32_Sword;
typedef unsigned int	Elf32_Word;
typedef unsigned short	Elf32_Half;

/*
 * This file uses snake case, but you can cast safely.
*/

typedef Elf32_Addr	elf32_addr; 
typedef Elf32_Off	elf32_off;
typedef Elf32_Sword	elf32_sword;
typedef Elf32_Word	elf32_word;
typedef Elf32_Half	elf32_half;

#define ET_NONE   0 	 /* No file type */
#define ET_REL 	  1 	 /* Relocatable file */
#define ET_EXEC   2 	 /* Executable file */
#define ET_DYN 	  3 	 /* Shared object file */
#define ET_CORE   4 	 /* Core file */
#define ET_LOPROC 0xff00 /* Processor-specific */
#define ET_HIPROC 0xffff /* Processor-specific */

#define ET_NONE  0  /* No machine */
#define EM_M32 	 1  /* AT&T WE 32100 */
#define EM_SPARC 2  /* SPARC */
#define EM_386 	 3  /* Intel Architecture */
#define EM_68K 	 4  /* Motorola 68000 */
#define EM_88K 	 5  /* Motorola 88000 */
#define EM_860 	 7  /* Intel 80860 */
#define EM_MIPS	 8  /* MIPS RS3000 Big-Endian */
#define EM_MIPS_RS4_BE 	10 /* MIPS RS4000 Big-Endian */
/* RESERVED 11-16 */

#define EV_NONE    0 /* Invalid versionn */
#define EV_CURRENT 1 /* Current version */

#define EI_MAG0  0 	/* File identification */
#define EI_MAG1  1 	/* File identification */ 
#define EI_MAG2  2 	/* File identification */
#define EI_MAG3  3 	/* File identification */
#define EI_CLASS 4	/* File class */
#define EI_DATA  5 	/* Data encoding */
#define EI_PAD   7 	/* Start of padding bytes */
#define EI_VERSION 6 	/* File version */
#define EI_NIDENT  16	/* Size of e_ident[] */

#define ELFMAG0 0x7f /* e_ident[EI_MAG0] */
#define ELFMAG1 ’E’  /* e_ident[EI_MAG1] */
#define ELFMAG2 ’L’  /* e_ident[EI_MAG2] */
#define ELFMAG3 ’F’  /* e_ident[EI_MAG3] */

#define ELFCLASSNONE 0 /* Invalid class */
#define ELFCLASS32   1 /* 32-bit objects */
#define ELFCLASS64   2 /* 64-bit objects */

#define ELFDATANONE 0 /* Invalid data encoding */
#define ELFDATA2LSB 1 /* See below */
#define ELFDATA2MSB 2 /* See below */

#define SHN_UNDEF 	0 	
#define SHN_LORESERVE	0xff00	
#define SHN_LOPROC	0xff00  
#define SHN_HIPROC	0xff1f  
#define SHN_ABS		0xfff1  
#define SHN_COMMON 	0xfff2  
#define SHN_HIRESERVE	0xffff  

#define SHT_NULL 	0 
#define SHT_PROGBITS 	1 
#define SHT_SYMTAB 	2 
#define SHT_STRTAB 	3 
#define SHT_RELA 	4 
#define SHT_HASH 	5 
#define SHT_DYNAMIC 	6 
#define SHT_NOTE 	7 
#define SHT_NOBITS 	8 
#define SHT_REL 	9 
#define SHT_SHLIB 	10
#define SHT_DYNSYM 	11
#define SHT_LOPROC 	0x70000000
#define SHT_HIPROC 	0x7fffffff
#define SHT_LOUSER 	0x80000000
#define SHT_HIUSER 	0xffffffff

#define SHF_WRITE	0x1
#define SHF_ALLOC	0x2
#define SHF_EXECINSTR	0x4
#define SHF_MASKPROC	0xf0000000 

#define STN_UNDEF	0 

#define ELF32_ST_BIND(i)   ((i)>>4)
#define ELF32_ST_TYPE(i)   ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define STB_LOCAL 	0 
#define STB_GLOBAL 	1 
#define STB_WEAK 	2 
#define STB_LOPROC 	13
#define STB_HIPROC 	15

#define STT_NOTYPE	0
#define STT_OBJECT	1
#define STT_FUNC	2
#define STT_SECTION	3
#define STT_FILE	4
#define STT_LOPROC	13
#define STT_HIPROC	15

#define PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB	5
#define PT_PHDR		6
#define PT_LOPROC 	0x70000000
#define PT_HIPROC 	0x7fffffff

struct elf32_ehdr {
	unsigned char e_ident[EI_NIDENT];

	elf32_half e_type;
	elf32_half e_machine;
	elf32_word e_version;
	elf32_addr e_entry;
	elf32_off  e_phoff;
	elf32_off  e_shoff;
	elf32_word e_flags;
	elf32_half e_ehsize;
	elf32_half e_phentsize;
	elf32_half e_phnum;
	elf32_half e_shentsize;
	elf32_half e_shnum;
	elf32_half e_shstrndx;
};
typedef struct elf32_ehdr Elf32_Ehdr;

struct elf32_shdr{
	elf32_word sh_name; 
	elf32_word sh_type; 
	elf32_word sh_flags;
	elf32_addr sh_addr;
	elf32_off  sh_offset;
	elf32_word sh_size;
	elf32_word sh_link;
	elf32_word sh_info;
	elf32_word sh_addralign;
	elf32_word sh_entsize;
};
typedef struct elf32_shdr Elf32_Shdr;

struct elf32_sym {
	elf32_word st_name; 
	elf32_addr st_value;
	elf32_word st_size;
	unsigned char st_info;
	unsigned char st_other;
	elf32_half st_shndx;
};
typedef struct elf32_sym Elf32_Sym;

struct elf32_rel{
	elf32_addr r_offset;
	elf32_word r_info;
};
typedef struct elf32_rel Elf32_Rel;

struct elf32_rela{
	elf32_addr r_offset;
	elf32_word r_info;
	elf32_sword r_addend;
};
typedef struct elf32_rela Elf32_Rela;

struct elf32_phdr{
	elf32_word p_type;
	elf32_off p_offset;
	elf32_addr p_vaddr;
	elf32_addr p_paddr;
	elf32_word p_filesz;
	elf32_word p_memsz;
	elf32_word p_flags;
	elf32_word p_align;
};
typedef struct elf32_phdr Elf32_Phdr;

#endif /* __H_SHL_ELF_H */