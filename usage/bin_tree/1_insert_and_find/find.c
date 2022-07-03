#include <stdio.h>
#include "../../../bin_tree.h"

struct obj {
	unsigned int data;
	struct tree_node bst_node;
};

/* similar to obj_cmp_node, but takes key */
static inline int obj_cmp_key(struct tree_node* node, const void* key){
	struct obj *ent;
	ent = get_entry(node, struct obj, bst_node);
	return ent->data - *(unsigned int*)key;
}

static inline int obj_cmp_node(
		struct tree_node* node0,
		struct tree_node* node1)
{
	return obj_cmp_key(node0, get_entry(node1, struct obj, bst_node));
}

#define arr_size(name) \
	sizeof(name) / sizeof(*name)


/*
 * THIS TREE IS CREATED BELOW
 *
 *              8
 *           /     \
 *          3       10
 *         / \       \
 *        1   6       14
 *           / \      /
 *          4   7    13
 */

int main(void){
	struct obj root_obj;
	struct tree_node* root = &(root_obj.bst_node);
	root_obj.data = 8;
	tree_init_node(root);

	unsigned int ins_vals[] = {3,1,6,4,7,10,14,13};
	struct obj objs[arr_size(ins_vals)];

	for(int i = 0; i < arr_size(ins_vals); i++){
		tree_init_node(&(objs[i].bst_node));
		objs[i].data = ins_vals[i];
		bst_insert(root, &(objs[i].bst_node), 
				obj_cmp_node);
	}
	
	unsigned int element = 6;
	struct tree_node* node = bst_find(root, &element, obj_cmp_key);
	if (node)
		printf("node located in addr:%p, data:%u\n", 
				get_entry(node, struct obj, bst_node),
				get_entry(node, struct obj, bst_node)->data);
	else
		printf("node is not present\n");
	return 0;
}
