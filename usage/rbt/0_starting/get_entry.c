#include <stdio.h>

#include "../../../shl_rbt.h"

struct obj {
	unsigned int data;
	/* blah_blah_blah will be used later */
	struct shl_rbt_node blah_blah_blah;
};

int main(void){
	/* the main idea of the nodes in this project is upcast.
	 * you have to understand the...
	 * ...shl_get_entry MACRO
	 * by given pointer to node, type, and name in structure,
	 * it can give you casted pointer to base struct.
	 * Here is an example.
	 */

	/* create object with specified data */
	struct obj object;
	object.data = 1234;
	/* this will simply init tree pointers with NULL
	 * note, that we pass the address of struct shl_rbt_node, and not
	 * struct obj. */
	struct shl_rbt_node* obj_node = &(object.blah_blah_blah);
	shl_rbt_init_node(obj_node);
	/* here is where magic happens. we UPCAST 
	 * 'struct shl_rbt_node*' to 'struct obj*'
	 * here is how it goes:
	 */
	struct obj* obj;
	/* pass the node, to get type from, 
	 * type that you want to cast to,
	 * and name of the struct */
	obj = shl_get_entry(obj_node, struct obj, blah_blah_blah);
	/* obj is equal to &object */
	printf("%u is obj data!\n", obj->data);

	return 0;
}
