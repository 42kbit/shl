#include <stdio.h>
#include "../rbt.h"

struct obj {
	unsigned int data;
	/* tree node */
	struct rbt_node rbt_node;
};

/* similar to obj_cmp_node, but takes key */
static inline int obj_cmp_key(struct rbt_node* node, const void* key){
	struct obj *ent;
	ent = get_entry(node, struct obj, rbt_node);
	return ent->data - *(unsigned int*)key;
}

static inline int obj_cmp_node(
		struct rbt_node* node0,
		struct rbt_node* node1)
{
	return obj_cmp_key(node0, get_entry(node1, struct obj, rbt_node));
}

#define arr_size(name) \
	sizeof(name) / sizeof(*name)

/*
 * THIS TREE IS CREATED BELOW
 *
 * COLOR VALUE IS PLACED NEXT
 * TO NODE WHERE
 * b - BLACK
 * r - RED
 *
 * node 3b for example:
 * 	node with VALUE 3
 * 	and	  COLOR BLACK
 *
 *          	6b
 *             /     \
 *            3r      8r
 *           /  \    /  \
 *          1b   4b 7b   10b
 *                      \
 *                      14r
 *                	
 *                	
 */

static inline void print_tree(struct rbt_node* root){
	struct rbt_node* iter = NULL;
	while (iter = rbt_next(root, iter, obj_cmp_node)){
		struct obj* entry = get_entry(iter, struct obj, rbt_node);
		printf("data: %d, clr: %d\n", entry->data, iter->color);
	}
}

int main(void){
	struct rbt_node* root = NULL;
	unsigned int ins_vals[] = {8,3,1,6,4,7,10,14};
	struct obj objs[arr_size(ins_vals)];

	for(int i = 0; i < arr_size(ins_vals); i++){
		rbt_init_node(&(objs[i].rbt_node));
		objs[i].data = ins_vals[i];
		rbt_insert(&root, &(objs[i].rbt_node), 
				obj_cmp_node);
	}
	print_tree(root);
	return 0;
}
