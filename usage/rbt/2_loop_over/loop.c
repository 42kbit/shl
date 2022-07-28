#include <stdio.h>
#define SHL_RBT_NOABSTRACTIONS
#include "../../../shl_rbt.h"

struct obj {
	unsigned int data;
	struct shl_rbt_node rbt_node;
};

/* similar to obj_cmp_node, but takes key */
static inline int obj_cmp_key(struct shl_rbt_node* node, const void* key,
		const void*){
	struct obj *ent;
	ent = shl_get_entry(node, struct obj, rbt_node);
	return ent->data - *(unsigned int*)key;
}

static inline int obj_cmp_node(
		struct shl_rbt_node* node0,
		struct shl_rbt_node* node1,
		const void*)
{
	return obj_cmp_key(node0, shl_get_entry(node1, struct obj, rbt_node),
			NULL);
}

static inline void obj_print_tree(struct shl_rbt_node* root){
	struct shl_rbt_node* iter = NULL;
	/* if iter is NULL, rbt_find_next will return tree_most_left */
	while (iter = shl_rbt_next_node(root, iter)){
		printf("%u\n", 
				shl_get_entry(iter, struct obj,
					rbt_node)->data);
	}
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
	struct shl_rbt_node* root = &(root_obj.rbt_node);
	root_obj.data = 8;
	shl_rbt_init_node(root);

	unsigned int ins_vals[] = {3,1,6,4,7,10,14,13};
	struct obj objs[arr_size(ins_vals)];

	for(int i = 0; i < arr_size(ins_vals); i++){
		objs[i].data = ins_vals[i];
		shl_rbt_insert_node(&root, &(objs[i].rbt_node), 
				obj_cmp_node);
	}
	
	unsigned int element = 6;
	struct shl_rbt_node* node =
		shl_rbt_find_node(root, &element, obj_cmp_key);
	if (node)
		printf("node located in addr:%p, data:%u\n", 
				shl_get_entry(node, struct obj, rbt_node),
				shl_get_entry(node, struct obj, 
					rbt_node)->data);
	else
		printf("node is not present\n");
	obj_print_tree(root);
	return 0;
}
