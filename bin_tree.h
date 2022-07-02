#ifndef _H_BIN_TREE_H
#define _H_BIN_TREE_H

#ifndef NULL
#define NULL ((void*)0)
#endif

#define get_entry(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

struct tree_node{
	struct tree_node *right, *left;
};

typedef int(*tree_cmp_node_t)(struct tree_node*, struct tree_node*);
typedef int(*tree_cmp_key_t)(struct tree_node*, const void* key);

static inline void tree_init_node(struct tree_node* root){
	root->right = root->left = NULL;
}

/* just for fun ;P */

#define __define_tree_most(dir) 			\
static inline struct tree_node* tree_most_##dir( 	\
		struct tree_node* root){ 		\
	struct tree_node* iter = root; 			\
	while (iter-> dir){ 				\
		iter = iter-> dir; 			\
	} 						\
	return iter; 					\
}						

__define_tree_most(left)
__define_tree_most(right)

static inline struct tree_node* bst_insert(
		struct tree_node* root,
		struct tree_node* node,
		tree_cmp_node_t cmp)
{
	int res = cmp(node, root);
	if (res > 0){
		if (!root->right)
			return (root->right = node);
		else
			return bst_insert(root->right, node, cmp);
	}
	else if (res < 0){
		if (!root->left)
			return (root->left = node);
		else
			return bst_insert(root->left, node, cmp);
	}
	return NULL;
}

static inline struct tree_node* bst_find(
		struct tree_node* root,
		void* key,
		tree_cmp_key_t kcmp
		)
{
	if (!root)
		return NULL;
	int res = kcmp(root, key);
	if (res == 0)
		return root;
	else if (res > 0)
		return bst_find(root->left, key, kcmp);
	return bst_find(root->right, key, kcmp);
}

static inline struct tree_node* bst_find_parent(
		struct tree_node* root,
		struct tree_node* node,
		tree_cmp_node_t cmp
		)
{
	if (node == root)
		return root;
	if (root->left == node || root->right == node)
		return root;
	int res = cmp(root, node);
	if (res > 0)
		return bst_find_parent(root->left, node, cmp);
	return bst_find_parent(root->right, node, cmp);

}



static inline struct tree_node* bst_next_find(
		struct tree_node* root,
		struct tree_node* from,
		tree_cmp_node_t cmp)
{
	if (!from)
		return tree_most_left(root);
	if (from->right)
		return tree_most_left(from->right);
	struct tree_node* parent = bst_find_parent(root, from, cmp);
	while (parent->left != from){
		from = parent;
		parent = bst_find_parent(root, parent, cmp);
		if (parent == from)
			return NULL;
	}
	return parent;
}

static inline struct tree_node* bst_prev_find(
		struct tree_node* root,
		struct tree_node* from,
		tree_cmp_node_t cmp)
{
	if (!from)
		return tree_most_right(root);
	if (from->right)
		return tree_most_right(from->left);
	struct tree_node* parent = bst_find_parent(root, from, cmp);
	while (parent->right != from){
		from = parent;
		parent = bst_find_parent(root, parent, cmp);
		if (parent == from)
			return NULL;
	}
	return parent;
}

#endif /* _H_BIN_TREE_H */
