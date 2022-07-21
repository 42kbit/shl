#ifndef _H_DICT_H
#define _H_DICT_H

#include "../../rbt.h"

#define DICT_NODE_DLEN 128
struct dict_node{
	char key[DICT_NODE_DLEN];
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

/* *node and *root assumed preallocated, just like with 
 * other structures in this project */

static inline void dict_insert(
		struct dict_node **dict_root,
		struct dict_node *node,
		void* value,
		void (*ins_func)(struct dict_node* node, void* value))
{
	struct rbt_node *root = NULL;
	if (!*dict_root){
		rbt_insert(&root, &(node->rbt_node), dict_cmp_node);
		*dict_root = node;
		if (ins_func)
			ins_func(node, value);
		return;
	}
	root = &((*dict_root)->rbt_node);
	rbt_insert(&root, &(node->rbt_node), dict_cmp_node);
	*dict_root = get_entry(root, struct dict_node, rbt_node);
	if (ins_func)
		ins_func(node, value);
}

static inline struct dict_node* dict_get(
		struct dict_node *root,
		char *key)
{
	if (!root)
		return NULL;
	struct rbt_node* found = rbt_find(&(root->rbt_node), key,
			dict_cmp_key);
	if (!found)
		return NULL;
	return get_entry(found, struct dict_node, rbt_node);
}

static inline int dict_remove(
		struct dict_node **root,
		char* key)
{
	struct dict_node *torem = dict_get(*root, key); 
	if (!torem)
		return -1;
	struct rbt_node
		*rbt_root = &((*root)->rbt_node),
		*rbt_torem = &(torem->rbt_node);
	rbt_remove(&rbt_root, &rbt_torem);
	*root = get_entry(rbt_root, struct dict_node, rbt_node);
	return 0;
}

#endif /* _H_DICT_H */
