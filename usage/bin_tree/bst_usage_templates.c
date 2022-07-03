#include <stdio.h>
#include "../bin_tree.h"

struct obj {
	unsigned int data;
	/* just insert node instance to begin with */
	struct tree_node bst_node;
};

/* For functions that require comparing nodes, you have to pass
 * function as argument. 
 * There is usually a funciton for each data type.
 *
 * There are two types of those:
 * 	Identify by key  (see tree_cmp_key_t)
 * 	Identify by node (see tree_cmp_node_t)
 * The return value is 
 * ==0 if nodes are equal
 * > 0 if first arg is greater
 * < 0 if first arg is less
 */

/* example for identify by key */
static inline int obj_cmp_key(struct tree_node* node, const void* key){
	struct obj *ent;
	ent = get_entry(node, struct obj, bst_node);
	return ent->data - *(unsigned int*)key;
}

/* example for identify by node */
static inline int obj_cmp_node(
		struct tree_node* node0,
		struct tree_node* node1)
{
	return obj_cmp_key(node0, get_entry(node1, struct obj, bst_node));
}

/* gets array size by name.
 * works for arrays with size info in type */
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

/* prints tree using for loop. you will get that later. */
void obj_bst_print(struct tree_node* root){
	struct tree_node* iter = NULL;
	while (iter = bst_find_next(root, iter, obj_cmp_node)){
		printf("%u\n", get_entry(iter, struct obj, bst_node)->data);
	}
}

int main(void){
	/* each tree has a root, create one to begin with. */
	struct obj root_obj;
	struct tree_node* root = &(root_obj.bst_node);
	root_obj.data = 8;
	/* init node right and left with NULL */
	tree_init_node(root);
	/* specify data in right order for tree nodes
	 * to create. */
	unsigned int ins_vals[] = {3,1,6,4,7,10,14,13};
	/* array of objs, here copy data */
	struct obj objs[arr_size(ins_vals)];
	for(int i = 0; i < arr_size(ins_vals); i++){
		tree_init_node(&(objs[i].bst_node));
		objs[i].data = ins_vals[i];
		/* insert tree node, using compare func specified */
		bst_insert(root, &(objs[i].bst_node), 
				obj_cmp_node);
	}

	obj_bst_print(root);
	return 0;
}
