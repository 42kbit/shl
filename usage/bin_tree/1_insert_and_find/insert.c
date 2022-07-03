#include <stdio.h>
#include "../../../bin_tree.h"

struct obj {
	unsigned int data;
	struct tree_node bst_node;
};

int obj_cmp_node(struct tree_node* node0, struct tree_node* node1){
	struct obj *obj0, *obj1;
	obj0 = get_entry(node0, struct obj, bst_node);
	obj1 = get_entry(node1, struct obj, bst_node);
	return obj0->data - obj1->data;
}

int main(void){
	struct obj obj0;
	obj0.data = 55;
	struct obj obj1;
	obj1.data = 44;

	struct tree_node* root = &(obj0.bst_node);
	tree_init_node(root);
	/*        55
	 *       /  \
	 *    NULL   NULL
	 */

	struct tree_node* obj1_node = &(obj1.bst_node);
	tree_init_node(obj1_node);

	bst_insert(root, obj1_node, obj_cmp_node);
	/*        55
	 *       /  \
	 *     44   NULL
	 */
	printf("%u is obj data!\n", 
			get_entry(root, struct obj, bst_node)->data);
	printf("%u is obj data!\n", 
			get_entry(root->left, struct obj, bst_node)->data);
	return 0;
}
