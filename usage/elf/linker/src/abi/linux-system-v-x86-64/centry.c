/* Does a PIC stack pointer load, then jumps to main() */
#include <lib/cmn.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <abi/aux.h>
#include <abi/so.h>
#include <abi/so_load.h>

#define PAGE_SIZE 4096

const char* __envp_find (const char** envp, const char* ename);

void __set_sp (void* new);

static char __proc_stack[mib(1)];
/* Generates R_X86_64_RELATIVE */
static char* __proc_stack_top = (char*)__proc_stack + mib(1);

int pre_main(int argc, const char* argv[], const char* envp[]){
	/* Works? YEAH
	*  Like shit? YEAH
	*  DO i no other way around? NO I DONT
	*  5 Hours of assembly debug was not worth it
	*  
	* So the stack pointer is set up cuz &__proc_stack_top
	* generates relocation, that is resolved by kernel upon load.
	* 
	* I dont know if that is a standartized way, but at this point i dont care.
	*/
	__set_sp((void*)&__proc_stack_top);

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
		printf("Invalid page size. Hardcoded %u, actual %u\n",
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
	shl_list_init_head (&libdirs_head);
		
	struct libdir default_path_node;
	libdir_add_search_path (&libdirs_head, &(default_path_node.list), "/lib");
	
	struct libdir ld_library_path_node;
	const char* ld_library_path = __envp_find (envp, "LD_LIBRARY_PATH");
	if (ld_library_path != NULL)
		libdir_add_search_path (&libdirs_head, &(ld_library_path_node.list), ld_library_path);
	
	const char* failname = NULL;
	if ((retval = load_so_deps (&aux_exec, &libdirs_head, &failname)) < EOK){
		printf("Dependency %s load error!\n", failname);
		return -retval;
	}
	
	/* Before giving control to executable, make sure to push argc, argv[] and envp[]
	 * according to System V ABI x86-64, but for now, we dont care.
	 */

	while (1);
	/* Will segfault for now due to lack of relocation mechanism */
	void (*entry)() = auxvals[AT_ENTRY]->a_un.a_ptr;
	entry();

	exit (0);
	return -1;
}