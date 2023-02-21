#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../../shl_rbt.h"

struct shl_tree;
struct shl_tree_node;
typedef struct shl_tree_node shl_tree_node_t;
typedef struct shl_tree shl_tree_t;

/* used for foreach loop */
typedef int (*shl_tree_traverse_func)(
		shl_tree_t* tree,
		shl_tree_node_t* node,
		void* user_data);

/* qsort style key compare func */
typedef int (*shl_tree_cmp)(const void*, const void*);

/* destroy callback function */
typedef void (*shl_tree_destroy)(
		shl_tree_t* tree,
		shl_tree_node_t* val,
		void* user_data);

struct shl_tree_node{
	void* key;
	void* data;
	shl_rbt_node_t rbt_node;
};

struct shl_tree{
	shl_tree_node_t* root;
	shl_tree_cmp key_cmp;
	shl_tree_destroy key_destroy, 
			 data_destroy;
};

#define shl_allocate_type(type) ((type*)malloc(sizeof(type)))
#define shl_allocate_types(type, n) ((type*)malloc(sizeof(type) * n))

static inline int shl_tree_node_cmp_full(
		shl_rbt_node_t* n0, shl_rbt_node_t* n1, const void* data)
{
	shl_tree_t* tree = (shl_tree_t*)data;
	shl_tree_node_t
		*e0 = shl_get_entry(n0, shl_tree_node_t, rbt_node),
		*e1 = shl_get_entry(n1, shl_tree_node_t, rbt_node);
	return tree->key_cmp(e0->key, e1->key);
}

static inline int shl_tree_key_cmp_full(
		shl_rbt_node_t* n0, const void* key, const void* user_data)
{
	shl_tree_t* tree = (shl_tree_t*)user_data;
	return tree->key_cmp(
			shl_get_entry(n0, shl_tree_node_t, rbt_node)->key,
			key);
}

static inline shl_tree_t* shl_tree_new_full(
		shl_tree_cmp key_cmp,
		shl_tree_destroy key_destroy,
		shl_tree_destroy data_destroy)
{
	shl_tree_t* newtree;
	if (!key_cmp)
		return NULL;
	if (!(newtree = shl_allocate_type(shl_tree_t)))
		return NULL;
	newtree->root = NULL;
	newtree->key_cmp = key_cmp;
	newtree->key_destroy = key_destroy;
	newtree->data_destroy = data_destroy;
	return newtree;
}

static inline shl_tree_t* shl_tree_new(
		shl_tree_cmp key_cmp)
{
	return shl_tree_new_full(key_cmp, NULL, NULL);
}

static inline shl_tree_node_t* shl_tree_new_node(
		void* key, void* data)
{
	shl_tree_node_t* newnode;
	if (!(newnode = shl_allocate_type(shl_tree_node_t)))
		return NULL;
	shl_rbt_init_node(&(newnode->rbt_node));
	newnode->key = key;
	newnode->data = data;
	return newnode;
}

static inline shl_tree_node_t* shl_tree_find(
		shl_tree_t* tree,
		void* key)
{
	if (!tree || !key || !(tree->root))
		return NULL;
	shl_rbt_node_t* found = shl_rbt_find_node(
			&(tree->root->rbt_node),
			key,
			shl_tree_key_cmp_full,
			tree);
	if (!found)
		return NULL;
	return shl_get_entry(found, shl_tree_node_t, rbt_node);
}

static inline shl_tree_node_t* shl_tree_insert(
		shl_tree_t* tree,
		void* key,
		void* data)
{
	if (!tree || !key || !data)
		return NULL;
	shl_rbt_node_t *root = NULL,
		*found = NULL;
	int flags = 0;

	shl_tree_node_t* newnode = shl_tree_new_node(key, data);
	if (!tree->root){
		shl_rbt_insert_node(&root, &(newnode->rbt_node),
				shl_tree_node_cmp_full, NULL, tree);
		tree->root = newnode;
		return newnode;
	}
	root = &(tree->root->rbt_node);
	found = shl_rbt_insert_node(&root, &(newnode->rbt_node),
			shl_tree_node_cmp_full, &flags, tree);
	tree->root = shl_get_entry(root, shl_tree_node_t, rbt_node);
	if (flags & SHL_RBT_FOUND){
		if (tree->key_destroy)
			tree->key_destroy(tree, newnode, NULL);
		free(newnode);
		shl_tree_node_t* found_entry = shl_get_entry(found,
			shl_tree_node_t, rbt_node);
		if (tree->data_destroy)
			tree->data_destroy(tree, found_entry, NULL);
		found_entry->data = data;
		return found_entry;
	}
	return newnode;
}

static inline void shl_tree_unlink(
		shl_tree_t* tree,
		shl_tree_node_t* node)
{
	if (!tree || !node)
		return;
	shl_rbt_node_t* root = &(tree->root->rbt_node);
	shl_rbt_remove_node(&root, &(node->rbt_node));
	if (root)
		tree->root = shl_get_entry(root, shl_tree_node_t, rbt_node);
	else
		tree->root = NULL;
}

static inline void shl_tree_deallocate_node_full(
		shl_tree_t* tree,
		shl_tree_node_t* node,
		void* user_data)
{
	if (tree->key_destroy)
		tree->key_destroy(tree, node, user_data);
	if (tree->data_destroy)
		tree->data_destroy(tree, node, user_data);
	free(node);
}

static inline void shl_tree_remove_full(
		shl_tree_t* tree,
		shl_tree_node_t* node,
		void* user_data)
{
	shl_tree_unlink(tree, node);
	shl_tree_deallocate_node_full(tree, node, user_data);
}

static inline void shl_tree_remove(
		shl_tree_t* tree, shl_tree_node_t* node)
{
	shl_tree_remove_full(tree, node, NULL);
}

static inline void __shl_tree_foreach(
		shl_tree_t* tree,
		shl_rbt_node_t* (*order)(shl_rbt_node_t*,
				shl_rbt_node_t*),
		shl_tree_traverse_func trav_func,
		void* user_data)
{
	if (!tree || !trav_func || !order)
		return;
	shl_rbt_node_t *iter = NULL;
	while ( (iter = order(&(tree->root->rbt_node), iter) ))
	{
		trav_func(
			tree,
			shl_get_entry(iter, shl_tree_node_t, rbt_node),
			user_data);
	}
}

static inline void shl_tree_foreach_inorder_full(
		shl_tree_t* tree,
		shl_tree_traverse_func trav_func,
		void* user_data)
{
	__shl_tree_foreach(tree, shl_rbt_next_node, trav_func, user_data);
}

static inline void shl_tree_foreach_inorder(
		shl_tree_t* tree,
		shl_tree_traverse_func trav_func)
{
	__shl_tree_foreach(tree, shl_rbt_next_node, trav_func, NULL);
}

static inline void shl_tree_foreach_preorder_full(
		shl_tree_t* tree,
		shl_tree_traverse_func trav_func,
		void* user_data)
{
	__shl_tree_foreach(tree, shl_rbt_prev_node, trav_func, user_data);
}

static inline void shl_tree_foreach_preorder(
		shl_tree_t* tree,
		shl_tree_traverse_func trav_func)
{
	__shl_tree_foreach(tree, shl_rbt_prev_node, trav_func, NULL);
}

static inline void shl_tree_remove_all_full(shl_tree_t* tree, void* user_data){
	if (!tree)
		return;
	while (tree->root){
		shl_tree_remove_full(tree, tree->root, user_data);
	}
}

static inline void shl_tree_remove_all(shl_tree_t* tree){
	shl_tree_remove_all_full(tree, NULL);
}

int dict_cmp(const void* _s1, const void* _s2){
	const char
		*s1 = (const char*) _s1,
		*s2 = (const char*) _s2;
	return strcmp(s1, s2);
}

static inline void dict_free_key(
		shl_tree_t* tree,
		shl_tree_node_t* val,
		void* user_data)
{
}

static inline void dict_free_data(
		shl_tree_t* tree,
		shl_tree_node_t* val,
		void* user_data)
{
}

static inline int dict_traverse_print(
		shl_tree_t* tree,
		shl_tree_node_t* node,
		void* user_data)
{
	return printf("%d\n", *(int*)node->data);
}

#define NKEYS 4

int main(void){
	struct shl_tree * tree = shl_tree_new_full(dict_cmp, dict_free_key, dict_free_data);

	const char* keys[NKEYS] = {"apple", "banana", "bruh", "Hello, world"};
	int data[NKEYS] = {10, 20, 30, 40};

	for (int i = 0; i < NKEYS; i++){
		shl_tree_insert(tree, (void*)keys[i], &data[i]);
	}
	shl_tree_foreach_inorder(tree, dict_traverse_print);

	shl_tree_remove_all (tree);
	return 0;
}
