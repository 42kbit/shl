#include <stdio.h>
#include "../bin_tree.h"

struct obj {
	unsigned int data;
	struct tree_node bst_node;
};

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

/* works for arrays with size info in type */
#define arr_size(name) \
	sizeof(name) / sizeof(*name)


/*
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
	
	struct tree_node* iter = NULL;
	unsigned int el = 3;
	while (iter = bst_prev_find(bst_find(root, &el, obj_cmp_key),
			       iter, obj_cmp_node)){
		struct obj* entry = get_entry(iter, struct obj, bst_node);
		printf("%u\n", entry->data);
	}
	return 0;
}
