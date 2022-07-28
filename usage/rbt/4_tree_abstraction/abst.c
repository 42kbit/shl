#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../shl_rbt.h"

struct obj {
	unsigned int data;
};

#define arr_size(name) \
	sizeof(name) / sizeof(*name)

#define MAX_KEY_LEN 256
int obj_cmp_key(const void* k0, const void* k1){
	return strncmp(k0, k1, MAX_KEY_LEN);
}

void obj_destroy_value(
		shl_tree_t* tree,
		shl_tree_node_t* val,
		void* user_data)
{
	printf("destroying value\n");
	free(val);
}

void obj_destroy_key(
		shl_tree_t* tree,
		shl_tree_node_t* val,
		void* user_data)
{
	printf("destroying key\n");
	free(val);
}

static inline shl_tree_node_t* obj_insert(shl_tree_t* tree,
		char* key, int value)
{
	char *newkey = malloc(sizeof(char) * strlen(key));
	strcpy(newkey, key);
	int *newdata = malloc(sizeof(int));
	*newdata = value;
	return shl_tree_insert(tree, newkey, newdata);
}

static inline int print_node(
		shl_tree_t* tree,
		shl_tree_node_t* node,
		void* user_data)
{
	printf("%d, %s, %d\n", node->rbt_node.color,
			node->key, *(int*)node->data);
	return 0;
}

int main(void){
	shl_tree_t* obj_tree = shl_tree_new_full(
			obj_cmp_key,
			obj_destroy_key,
			obj_destroy_value
			);
	obj_insert(obj_tree, "hell", 5);
	obj_insert(obj_tree, "hell", 1234);
	obj_insert(obj_tree, "wow", 10);
	obj_insert(obj_tree, "wow0", 1033);
	obj_insert(obj_tree, "wow1", 1033);
	obj_insert(obj_tree, "wow2", 1033);
	obj_insert(obj_tree, "wow3", 1033);
	obj_insert(obj_tree, "wow4", 1033);
	obj_insert(obj_tree, "wow5", 1033);
	obj_insert(obj_tree, "wow6", 1033);

	shl_tree_foreach_inorder(obj_tree, print_node);
	shl_tree_remove_all(obj_tree);

	return 0;
}
