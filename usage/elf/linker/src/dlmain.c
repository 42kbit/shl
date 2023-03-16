#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include "../../../../shl_elf.h"
#include "../../../../shl_list.h"

#include "string.h"
#include "aux.h"
#include "syscall.h"
#include "io.h"
#include "cmn.h"
#include "so.h"

#define PAGE_SIZE 4096

int main(int argc, const char* argv[], const char* envp[]){

	/* Seek end of envp[] */
	const char** iter;
	for (iter = envp; *iter != NULL; iter++)
		;
	/* Write auxv into auxvals to easy access. */
	struct auxv	*auxp = (struct auxv*)(iter + 1),
			*auxvals[AT_NTYPES] = {NULL};
	for (struct auxv* iter = auxp;
		iter->a_type != AT_NULL;
		iter++)
	{
		if (is_valid_atype(iter)){
			auxvals[iter->a_type] = iter;
		}
	}

	const struct auxv* auxpg = auxvals[AT_PAGESZ];
	if(auxpg != NULL && auxpg->a_un.a_val != PAGE_SIZE){
		printf("Invalid page size. Hardcoded %u, actual %u",
			PAGE_SIZE, auxpg->a_un.a_val);
		return -1;
	}

	/* Represents executable given by operating system on startup.
	 * LOAD type segments are already loaded.
	*/
	struct so_mem_desc aux_exec;
	int retval;
	if ((retval = init_so_mem_desc (&aux_exec, auxvals[AT_PHDR]->a_un.a_ptr,
				auxvals[AT_PHNUM]->a_un.a_val)) < EOK){
		printf("Init from aux error!\n");
		return -retval;
	}
	
	struct shl_list_node libdirs_head;
	/* Init path */
	shl_list_init_head (&libdirs_head);
		
	struct libdir default_path = {
		.path = "/lib"
	};
	shl_list_init_node (&(default_path.list));
	shl_list_insert_safe (&libdirs_head, &(default_path.list));

	struct libdir library_path = {
		.path = getenv(envp, "LD_LIBRARY_PATH")
	};
	if (library_path.path != NULL){
		shl_list_init_node (&(library_path.list));
		shl_list_insert_safe (&libdirs_head, &(library_path.list));
	}
	
	const char* failname = NULL;
	if ((retval = load_so_deps (&aux_exec, &libdirs_head, &failname)) < EOK){
		printf("Dependency %s load error!\n", failname);
		return -retval;
	}
	
	/* Before giving control to executable, make sure to push argc, argv[] and envp[]
	 * according to System V ABI x86-64, but for now, we dont care.
	 */

	/* Will segfault for now due to lack of relocation mechanism */
	void (*entry)() = auxvals[AT_ENTRY]->a_un.a_ptr;
	entry();

	return EOK;
}