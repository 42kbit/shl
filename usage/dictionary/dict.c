/* quite unefficent i guess, but im too dumb for implementing the
 * hashing algorithm */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../rbt.h"

#define DICT_NODE_DLEN 128
struct dict_node{
	char key[DICT_NODE_DLEN];
	int value;
	struct rbt_node rbt_node;
};

static inline int dict_cmp_key(struct rbt_node* node0, 
		const void* node1)
{
	struct dict_node* entry = get_entry(node0, struct dict_node, 
			rbt_node);
	return strncmp(entry->key, (const char*)node1, DICT_NODE_DLEN);
}

static inline int dict_cmp_node(struct rbt_node* node0, 
		struct rbt_node* node1)
{
	return dict_cmp_key(node0, get_entry(node1, struct dict_node,
				rbt_node)->key);
}

static inline void dict_free(struct rbt_node* node){
	struct dict_node* entry = get_entry(node, struct dict_node, rbt_node);
	free(entry);
}

#define arr_cnt(x) \
	sizeof(x) / sizeof(*x)

static inline void dict_insert(
		struct rbt_node** dict,
		char* key,
		int value)
{
	struct rbt_node* rbt_node = rbt_find(*dict, key,
			dict_cmp_key);
	if (rbt_node){
		struct dict_node* dnode = get_entry(rbt_node,
				struct dict_node, rbt_node);
		dnode->value = value;
		return;
	}
	struct dict_node* new_node = (struct dict_node*)malloc(
			sizeof(struct dict_node));
	strncpy(new_node->key, key, DICT_NODE_DLEN * sizeof(char));
	new_node->value = value;
	rbt_insert(dict, &(new_node->rbt_node), dict_cmp_node); 
}
static inline int dict_get(struct rbt_node* dict, char* key){
	struct rbt_node* rbt_node = rbt_find(dict, key, dict_cmp_key);
	if (!rbt_node)
		return -1;
	struct dict_node* dnode = get_entry(rbt_node, struct dict_node,
			rbt_node);
	return dnode->value;
}
static inline int dict_remove(struct rbt_node** dict, char* key){
	struct rbt_node* rbt_node = rbt_find(*dict, key, dict_cmp_key);
	if (!rbt_node)
		return -1;
	struct dict_node* dnode = get_entry(rbt_node, struct dict_node,
			rbt_node);
	rbt_remove(dict, &rbt_node);
	free(dnode);
}

int main(void){
	struct rbt_node* root = NULL;
	dict_insert(&root, "apple", 5);
	dict_insert(&root, "banana", 12);
	dict_insert(&root, "bruh", 999);
	dict_insert(&root, "bruh", 52);
	printf("%d\n", dict_get(root, "apple"));
	printf("%d\n", dict_get(root, "banana"));
	printf("%d\n", dict_get(root, "bruh"));
	dict_remove(&root, "banana");
	dict_remove(&root, "apple");
	dict_remove(&root, "bruh");
	return 0;
}
