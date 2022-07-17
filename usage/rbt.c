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

static inline void print_tree(struct rbt_node* root){
	struct rbt_node* iter = NULL;
	while (iter = rbt_next(root, iter, obj_cmp_node)){
		struct obj* entry = get_entry(iter, struct obj, rbt_node);
		struct obj* parent = get_entry(iter->parent, 
				struct obj, rbt_node);
		struct obj* left = get_entry(iter->left, 
				struct obj, rbt_node);
		struct obj* right = get_entry(iter->right, 
				struct obj, rbt_node);
		printf("data: %d, clr: %d," "parent: %d, left: %d, right %d\n", 
		entry->data, iter->color,
		(iter->parent? parent->data : -1),
		(iter->left? left->data : -1),
		(iter->right? right->data : -1));
	}
}

static inline void obj_remove_key(
		struct rbt_node** root,
		unsigned int key)
{
	struct rbt_node *torem = rbt_find(*root, &key, obj_cmp_key);
	rbt_remove(root, &torem);
}

int main(void){
	/* testing code */
	for(int i = 0; i < 0x7fffffff; i++){
	struct rbt_node* root = NULL;
	unsigned int obj_vals[256*256];
	for (int i = 0; i < arr_size(obj_vals); i++)
		obj_vals[i] = i;
	struct obj objs[arr_size(obj_vals)];
	for (int i = 0; i < arr_size(obj_vals); i++){
		objs[i].data = obj_vals[i];
		rbt_init_node(&(objs[i].rbt_node));
		rbt_insert(&root, &(objs[i].rbt_node), obj_cmp_node);
	}

	srand(time(NULL));
	printf("iter: %d\n", i);
	for (int i = 0; i < arr_size(obj_vals)*2; i++){
		int val = 1 + rand() % 12;
		obj_remove_key(&root, val);
	}
	}
	/*

	struct rbt_node* root = NULL;
	unsigned int obj_vals[32];
	for (int i = 0; i < arr_size(obj_vals); i++)
		obj_vals[i] = i;
	struct obj objs[arr_size(obj_vals)];
	for (int i = 0; i < arr_size(obj_vals); i++){
		objs[i].data = obj_vals[i];
		rbt_init_node(&(objs[i].rbt_node));
		rbt_insert(&root, &(objs[i].rbt_node), obj_cmp_node);
	}
	obj_remove_key(&root, 9);
	obj_remove_key(&root, 2);
	obj_remove_key(&root, 4);
	obj_remove_key(&root, 10);
	obj_remove_key(&root, 6);
	obj_remove_key(&root, 7);
	
	print_tree(root);
	*/

	return 0;
}
