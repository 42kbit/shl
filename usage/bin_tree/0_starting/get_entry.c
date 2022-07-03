#include <stdio.h>
#include "../../../bin_tree.h"

struct obj {
	unsigned int data;
	/* blah_blah_blah will be used later */
	struct tree_node blah_blah_blah;
};

int main(void){
	/* the main idea of the nodes in this project is upcast.
	 * you have to understand the...
	 * ...get_entry MACRO (defined in list.h and bin_tree.h)
	 * by given pointer to node, type, and name in structure,
	 * it can give you casted pointer to base struct.
	 * Here is an example.
	 */

	/* create object with specified data */
	struct obj object;
	object.data = 1234;
	/* this will simply init tree->right and tree->left with NULL
	 * note, that we pass the address of struct tree_node, and not
	 * struct obj. */
	struct tree_node* obj_node = &(object.blah_blah_blah);
	tree_init_node(obj_node);
	/* here is where magic happens. we UPCAST 
	 * 'struct tree_node*' to 'struct obj*'
	 * here is how it goes:
	 */
	struct obj* obj;
	/* pass the node, to get type from, 
	 * type that you want to cast to,
	 * and name of the struct */
	obj = get_entry(obj_node, struct obj, blah_blah_blah);
	/* obj is equal to &object */
	printf("%u is obj data!\n", obj->data);

	return 0;
}
