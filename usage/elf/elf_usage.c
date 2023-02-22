#include <stdio.h>
#include <stdlib.h>
#include "../../shl_elf.h"

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
	
	printf("%s\n", (char*)mem);
	struct  elf32_ehdr* p = mem;
	printf("%s\n", p->e_ident);

	free(mem);
	return 0;
}