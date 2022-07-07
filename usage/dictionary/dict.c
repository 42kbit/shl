/* quite unefficent i guess, but im too dumb for implementing the
 * hashing algorithm */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../bin_tree.h"

#define DICT_NODE_DLEN 128
struct dict_node{
	char data[DICT_NODE_DLEN];
	struct tree_node bst_node;
};

static inline int dict_cmp_key(struct tree_node* node0, 
		void* node1)
{
	struct dict_node* entry = get_entry(node0, struct dict_node, 
			bst_node);
	return strncmp(entry->data, (const char*)node1, DICT_NODE_DLEN);
}

static inline int dict_cmp_node(struct tree_node* node0, 
		struct tree_node* node1)
{
	return dict_cmp_key(node0, get_entry(node1, struct dict_node,
				bst_node)->data);
}

static inline void dict_free(struct tree_node* node){
	struct dict_node* entry = get_entry(node, struct dict_node, bst_node);
	free(entry);
}

#define arr_cnt(x) \
	sizeof(x) / sizeof(*x)

int main(void){
	struct tree_node* root = NULL;
	char *strings[] = {"hello", "world", "random", 
		"banana", "apple", "ca;lsdfj", "da;osdfkj", "zasfadfafs", "y", "z", "x"};
	struct dict_node** dnodes = (struct dict_node**)malloc(
			sizeof(struct dict_node*) * arr_cnt(strings));
	for (int i = 0; i < arr_cnt(strings); i++){
		dnodes[i] = (struct dict_node*)malloc(
				sizeof(struct dict_node));
		strncpy(dnodes[i]->data, strings[i], DICT_NODE_DLEN);
		bst_insert(&root, &(dnodes[i]->bst_node), dict_cmp_node);
	}
	struct tree_node* iter = NULL;
	/* prints in alphabetical order ;) */
	while (iter = bst_find_next(root, iter, dict_cmp_node)){
		struct dict_node* entry = get_entry(iter, struct dict_node,
				bst_node);
		printf("%s\n", entry->data);
	}
	return 0;
}
