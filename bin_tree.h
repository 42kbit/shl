#ifndef _H_BIN_TREE_H
#define _H_BIN_TREE_H

/* bst stands for binary search tree.
 * funcitons with __ prefix is INTERNAL */

/* self explanaroty */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* upcast node to type, see usage/bin_tree/0_starting */
#define get_entry(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

struct tree_node{
	struct tree_node *right, *left;
};

/* tree_cmp_node_t used for compare two nodes, this signature is 
 * passed by functions that require comparing inbetween nodes. */
typedef int(*tree_cmp_node_t)(struct tree_node*, struct tree_node*);
/* tree_cmp_key_t used for compare node with a key. */
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
/* used as tree_most_left(root) or 
 * tree_most_right(root), and simply will find most [INSERT DIR HERE]
 * node*/
__define_tree_most(left)
__define_tree_most(right)

/* insert in bst, see usate/bin_tree/1_insert_and_find/insert.c
 * for example */
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

/* find node by key in bst. kcmp is funciton that compares key with node,
 * see example in usage/bin_tree/1_insert_and_find/find.c */
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

/* finds parent of node, from root. */
static inline struct tree_node* bst_find_parent(
		struct tree_node* root,
		struct tree_node* node,
		tree_cmp_node_t cmp
		)
{
	if (!node)
		return NULL;
	if (node == root)
		return NULL;
	if (root->left == node || root->right == node)
		return root;
	int res = cmp(root, node);
	if (res > 0)
		return bst_find_parent(root->left, node, cmp);
	return bst_find_parent(root->right, node, cmp);
}

/* gets next from current node, using iterator 
 * see usage/bin_tree/2_loop_over/loop.c (function obj_print_tree) */
static inline struct tree_node* bst_find_next(
		struct tree_node* root,
		struct tree_node* from,
		tree_cmp_node_t cmp)
{
	if (!root)
		return NULL;
	if (!from)
		return tree_most_left(root);
	if (from->right)
		return tree_most_left(from->right);
	struct tree_node* parent = bst_find_parent(root, from, cmp);
	while (parent && parent->left != from){
		from = parent;
		parent = bst_find_parent(root, parent, cmp);
	}
	return parent;
}

/* gets next prev current node, using iterator */
static inline struct tree_node* bst_find_prev(
		struct tree_node* root,
		struct tree_node* from,
		tree_cmp_node_t cmp)
{
	if (!root)
		return NULL;
	if (!from)
		return tree_most_right(root);
	if (from->right)
		return tree_most_right(from->left);
	struct tree_node* parent = bst_find_parent(root, from, cmp);
	while (parent && parent->right != from){
		from = parent;
		parent = bst_find_parent(root, parent, cmp);
	}
	return parent;
}

static inline struct tree_node* __tree_get_replacer(
		struct tree_node* node)
{
	if (node->right)
		return tree_most_left(node->right);
	if (node->left)
		return tree_most_right(node->left);
	return NULL;
}

static inline void __tree_repl_child(
		struct tree_node* parent,
		struct tree_node* child,
		struct tree_node* replace_to)
{
	if (parent->left == child)
		parent->left = replace_to;
	else if (parent->right == child)
		parent->right = replace_to;
}

static inline struct tree_node* bst_remove(
		struct tree_node* root,
		struct tree_node* node,
		tree_cmp_node_t cmp)
{
	/* i dont think that i can continue... continue?
	 * to have done things i have done... in the name of progress and...
	 * healing... it was madness. i can see that now.. madness? MADNESS! 
	 * there is no hope..
	 * leave now... leave while you still have
	 * HOPE */
	if (!root || !node)
		return NULL;
	struct tree_node *repl = NULL, *repl_parent = NULL,
		*node_parent = NULL;
	repl = __tree_get_replacer(node);
	repl_parent = bst_find_parent(root, repl, cmp);
	node_parent = bst_find_parent(root, node, cmp);

	if (repl_parent && repl)
		__tree_repl_child(repl_parent, repl, NULL);
	if (node_parent)
		__tree_repl_child(node_parent, node, repl);
	if (repl){
		/* if node is by coincidence a parent of repl
		 * the direction was cleared by this line:
		 * __tree_repl_child(repl_parent, repl, NULL);
		 */
		if (node->right)
			repl->right = node->right;
		if (node->left)
			repl->left = node->left;
	}
	node->left = node->right = NULL;
	return repl;
}

static inline void bst_remove_subtree(
		struct tree_node* root,
		struct tree_node* stree,
		tree_cmp_node_t cmp)
{
	/* TODO: do todo */
	struct tree_node* stree_parent =
		bst_find_parent(root, stree, cmp), *stree_cpy = stree;
	struct tree_node* iter = NULL;
	while (iter = bst_find_next(stree, NULL, cmp)){
		stree = bst_remove(stree, stree, cmp);
	}
	if (stree_parent){
		__tree_repl_child(stree_parent, stree_cpy, NULL);
	}
}

#endif /* _H_BIN_TREE_H */
