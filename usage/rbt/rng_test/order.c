#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "../../../shl_rbt.h"

struct obj {
	unsigned int data;
	/* tree node */
	struct shl_rbt_node rbt_node;
};

/* similar to obj_cmp_node, but takes key */
static inline int obj_cmp_key(
		struct shl_rbt_node* node,
		const void* key,
		const void* _)
{
	struct obj *ent;
	ent = shl_get_entry(node, struct obj, rbt_node);

	return ent->data - *(unsigned int*)key;
}

static inline int obj_cmp_node(
		struct shl_rbt_node* node0,
		struct shl_rbt_node* node1,
		const void* _)
{
	return obj_cmp_key(node0, shl_get_entry(node1, struct obj, rbt_node),
			NULL);
}

#define arr_size(name) \
	sizeof(name) / sizeof(*name)

/*
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
 *          	  6b
 *             /     \
 *            3r      8r
 *           /  \    /  \
 *          1b   4b 7b   12b
 *         /             / \
 *        0r           10r  14r
 *                	
 *                	
 */

static inline void print_tree(struct shl_rbt_node* root){
	struct shl_rbt_node* iter = NULL;
	while ( (iter = shl_rbt_next_node(root, iter)) ){
		struct obj* entry = shl_get_entry(iter, struct obj, rbt_node);
		struct obj* parent = shl_get_entry(iter->parent, 
				struct obj, rbt_node);
		struct obj* left = shl_get_entry(iter->left, 
				struct obj, rbt_node);
		struct obj* right = shl_get_entry(iter->right, 
				struct obj, rbt_node);
		printf("data: %d, clr: %d," "parent: %d, left: %d, right %d\n", 
		entry->data, iter->color,
		(iter->parent? parent->data : -1),
		(iter->left? left->data : -1),
		(iter->right? right->data : -1));
	}
}

static inline void obj_remove_key(
		struct shl_rbt_node** root,
		unsigned int key)
{
	struct shl_rbt_node *torem = shl_rbt_find_node(*root, &key, obj_cmp_key);
	shl_rbt_remove_node(root, torem);
}

int main(void){
	/* testing code */
	struct shl_rbt_node* root = NULL;
	unsigned int obj_vals[21];
	for (int i = 0; i < arr_size(obj_vals); i++)
		obj_vals[i] = i;
	struct obj objs[arr_size(obj_vals)];
	for (int i = 0; i < arr_size(obj_vals); i++){
		objs[i].data = obj_vals[i];
		shl_rbt_init_node(&(objs[i].rbt_node));
		shl_rbt_insert_node(&root, &(objs[i].rbt_node), obj_cmp_node);
	}
	print_tree(root);
}
