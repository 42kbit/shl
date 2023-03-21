#include <abi/syscalls.h>
#include <lib/cmn.h>

struct mem_frame {
	size_t memsize;
	char mem[];
};

/* VEEEERY shity but will go fow now */
void* malloc (size_t nbytes) {
	struct mem_frame* mem = sys_mmap (NULL, nbytes, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (mem == MAP_FAILED)
		return NULL;

	return mem->mem;
}

void free (void* _mem){
	struct mem_frame* mem = container_of (_mem, struct mem_frame, mem);
	sys_munmap (mem->mem, mem->memsize);
}