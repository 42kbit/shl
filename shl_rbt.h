#ifndef _H_SHL_RB_TREE_H
#define _H_SHL_RB_TREE_H

/* define SHL_RBT_ABSTRACTIONS to include abstractions */

/* MINIMIZED API START */

struct shl_rbt_node{
	struct shl_rbt_node *right, *left, *parent;
	char color;
};
typedef struct shl_rbt_node shl_rbt_node_t;

typedef int(*shl_rbt_cmp_key)(struct shl_rbt_node* node,
		const void* key, const void* user_data);
typedef int(*shl_rbt_cmp_node_full)(struct shl_rbt_node*,
		struct shl_rbt_node*, const void* user_data);

static void shl_rbt_init_node (struct shl_rbt_node* node);

static struct shl_rbt_node* shl_rbt_find_node_full(
		struct shl_rbt_node* root,
		const void* key,
		shl_rbt_cmp_key kcmp,
		const void* user_data);

static struct shl_rbt_node* shl_rbt_find_node(
		struct shl_rbt_node* root,
		const void* key,
		shl_rbt_cmp_key kcmp);

static struct shl_rbt_node* shl_rbt_parent(
		struct shl_rbt_node* child);

static struct shl_rbt_node* shl_rbt_insert_node(
		struct shl_rbt_node** root,
		struct shl_rbt_node* node,
		shl_rbt_cmp_node_full ncmpfull);

static struct shl_rbt_node* shl_rbt_insert_node_full(
		struct shl_rbt_node** root,
		struct shl_rbt_node* node,
		shl_rbt_cmp_node_full ncmpfull,
		void* user_data);

static struct shl_rbt_node* shl_rbt_next_node(
		struct shl_rbt_node* root,
		struct shl_rbt_node* from);

static struct shl_rbt_node* shl_rbt_prev_node(
		struct shl_rbt_node* root,
		struct shl_rbt_node* from);

static void shl_rbt_remove_node(
		struct shl_rbt_node** root,
		struct shl_rbt_node* node);

/* MINIMIZED API END */
/* ABSTRACTED API START */

#ifdef SHL_RBT_ABSTRACTIONS

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

static inline shl_tree_t* shl_tree_new_full(
		shl_tree_cmp key_cmp,
		shl_tree_destroy key_destroy,
		shl_tree_destroy data_destroy);

static inline shl_tree_t* shl_tree_new(
		shl_tree_cmp key_cmp);

static inline shl_tree_node_t* shl_tree_new_node_full(
		void* key, void* data);

static inline shl_tree_node_t* shl_tree_find(
		shl_tree_t* tree,
		void* key);

static inline shl_tree_node_t* shl_tree_insert(
		shl_tree_t* tree,
		void* key,
		void* data);

static inline void shl_tree_unlink(
		shl_tree_t* tree,
		shl_tree_node_t* node);

static inline void shl_tree_remove_full(
		shl_tree_t* tree,
		shl_tree_node_t* node,
		void* user_data);

static inline void shl_tree_remove(
		shl_tree_t* tree, shl_tree_node_t* node);

static inline void shl_tree_foreach_inorder_full(
		shl_tree_t* tree,
		shl_tree_traverse_func trav_func,
		void* user_data);

static inline void shl_tree_foreach_inorder(
		shl_tree_t* tree,
		shl_tree_traverse_func trav_func);

static inline void shl_tree_foreach_preorder_full(
		shl_tree_t* tree,
		shl_tree_traverse_func trav_func,
		void* user_data);

static inline void shl_tree_foreach_preorder(
		shl_tree_t* tree,
		shl_tree_traverse_func trav_func);

static inline void shl_tree_remove_all_full(
		shl_tree_t* tree, 
		void* user_data);

static inline void shl_tree_remove_all(shl_tree_t* tree);

#endif /* SHL_RBT_ABSTRACTIONS */
/* ABSTRACTED API END */

#ifndef NULL
#define NULL ((void*)0)
#endif

#define shl_get_entry(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

#define RBT_BLACK 0
#define RBT_RED 1

#define is_red(node) ((node) && ((node)->color == RBT_RED))
#define is_black(node) (!(node) || ((node)->color == RBT_BLACK))

#define __define_tree_most(dir) 			\
static inline struct shl_rbt_node* __tree_most_##dir( 	\
		struct shl_rbt_node* root){ 		\
	struct shl_rbt_node* iter = root; 		\
	while (iter-> dir){ 				\
		iter = iter-> dir; 			\
	} 						\
	return iter; 					\
}						
/* used as tree_most_left(root) or 
 * tree_most_right(root), and simply will find most [INSERT DIR HERE]
 * node, (done just for fun) */
__define_tree_most(left)
__define_tree_most(right)

static inline void shl_rbt_init_node(
		struct shl_rbt_node* node)
{
	node->left = node->right = node->parent = NULL;
	node->color = RBT_RED;
}

static inline struct shl_rbt_node* shl_rbt_find_node(
		struct shl_rbt_node* root,
		const void* key,
		shl_rbt_cmp_key kcmp)
{
	return shl_rbt_find_node_full(root, key, kcmp, NULL);
}

static inline struct shl_rbt_node* shl_rbt_find_node_full(
		struct shl_rbt_node* root,
		const void* key,
		shl_rbt_cmp_key kcmp,
		const void* user_data)
{
	if (!root)
		return NULL;
	int res = kcmp(root, key, user_data);
	if (res == 0)
		return root;
	else if (res > 0)
		return shl_rbt_find_node_full(root->left, key,
				kcmp, user_data);
	return shl_rbt_find_node_full(root->right, key,
			kcmp, user_data);
}

static inline struct shl_rbt_node* shl_rbt_parent(
		struct shl_rbt_node* child)
{
	if (!child)
		return NULL;
	return child->parent;
}

static inline void __shl_rbt_turn_left(
		struct shl_rbt_node** node)
{
	if (!((*node)->right))
		return;
	struct shl_rbt_node *n_node = (*node)->right,
			*n_right = n_node->left,
			*parent = shl_rbt_parent(*node);
	if (parent){
		if (parent->left == *node)
			parent->left = n_node;
		else
			parent->right = n_node;
	}
	if (n_right)
		n_right->parent = *node;
	n_node->parent = parent;
	(*node)->parent = n_node;
	(*node)->right = n_right;
	n_node->left = *node;
	*node = n_node;
}

static inline void __shl_rbt_turn_right(
		struct shl_rbt_node** node)
{
	if (!((*node)->left))
		return;
	struct shl_rbt_node *n_node = (*node)->left,
			*n_left = n_node->right,
			*parent = shl_rbt_parent(*node);
	if (parent){
		if (parent->left == *node)
			parent->left = n_node;
		else
			parent->right = n_node;
	}
	if (n_left)
		n_left->parent = *node;
	n_node->parent = parent;
	(*node)->parent = n_node;
	(*node)->left = n_left;
	n_node->right = *node;
	*node = n_node;
}

static inline struct shl_rbt_node* __shl_rbt_add(
		struct shl_rbt_node* root,
		struct shl_rbt_node* node,
		shl_rbt_cmp_node_full ncmpfull,
		void* user_data)
{
	int cmp;
	if (!ncmpfull)
		return NULL;
	cmp = ncmpfull(root, node, user_data);
	if (cmp < 0){
		if (!root->right){
			node->parent = root;
			return (root->right = node);
		}
		return __shl_rbt_add(root->right, node, ncmpfull,
				user_data);
	}
	else if (cmp > 0){
		if (!root->left){
			node->parent = root;
			return (root->left = node);
		}
		return __shl_rbt_add(root->left, node, ncmpfull,
				user_data);
	}
	return root;
}

static inline void __shl_rbt_push_blackness(
		struct shl_rbt_node* from)
{
	/* no validation for now */
	from->color = RBT_RED;
	if (from->left)
		from->left->color = RBT_BLACK;
	if (from->right)
		from->right->color = RBT_BLACK;
}

/* works like magic but i'm proud of it */
static inline void __shl_rbt_added_relocate(
		struct shl_rbt_node** root,
		struct shl_rbt_node* node)
{
	struct shl_rbt_node *parent = shl_rbt_parent(node),
			*gparent = shl_rbt_parent(parent),
			*uncle = NULL;
	int turn_right = 0;
	if (!gparent || !parent)
		return;
	turn_right = (gparent->left == parent);
	uncle = turn_right? gparent->right : gparent->left;
	if (is_red(parent) && is_red(node)){
		/* maybe !is_red is not needed, but idk */
		if (!uncle || !is_red(uncle)){
			if (parent->right == node && turn_right){
				__shl_rbt_turn_left(&parent);
				node = parent->left;
			}
			else if(parent->left == node && !turn_right){
				__shl_rbt_turn_right(&parent);
				node = parent->right;
			}
		}
		if (uncle && is_red(uncle)){
			__shl_rbt_push_blackness(gparent);
			__shl_rbt_added_relocate(root, gparent);
		}
		else if (!uncle || !is_red(uncle)){
			gparent->color = RBT_RED;
			parent->color = RBT_BLACK;
			node->color = RBT_RED;
			/* set new root, if grandparent is equal to root */
			int set_after = 0;
			if (*root == gparent)
				set_after = 1;
			turn_right? __shl_rbt_turn_right(&gparent):
				__shl_rbt_turn_left(&gparent);
			if (set_after)
				*root = gparent;
		}
		
	}
	(*root)->color = RBT_BLACK;
}

/* add + validate to make binary search tree
 * red and black tree. */
static inline struct shl_rbt_node* shl_rbt_insert_node_full(
		struct shl_rbt_node** root,
		struct shl_rbt_node* node,
		shl_rbt_cmp_node_full ncmpfull,
		void* user_data)
{
	shl_rbt_init_node(node);
	if (!*root){
		node->color = RBT_BLACK;
		*root = node;
		return *root;
	}
	struct shl_rbt_node* added = __shl_rbt_add(*root, node, 
			ncmpfull, user_data);
	__shl_rbt_added_relocate(root, added);
	return added;
}

static inline struct shl_rbt_node* shl_rbt_insert_node(
		struct shl_rbt_node** root,
		struct shl_rbt_node* node,
		shl_rbt_cmp_node_full ncmpfull)
{
	return shl_rbt_insert_node_full(root, node, ncmpfull, NULL);
}

static inline struct shl_rbt_node* shl_rbt_next_node(
		struct shl_rbt_node* root,
		struct shl_rbt_node* from)
{
	if (!root)
		return NULL;
	if (!from)
		return __tree_most_left(root);
	if (from->right)
		return __tree_most_left(from->right);
	struct shl_rbt_node* parent = shl_rbt_parent(from);
	while (parent && parent->left != from){
		from = parent;
		parent = shl_rbt_parent(parent);
	}
	return parent;
}

static inline struct shl_rbt_node* shl_rbt_prev_node(
		struct shl_rbt_node* root,
		struct shl_rbt_node* from)
{
	if (!root)
		return NULL;
	if (!from)
		return __tree_most_right(root);
	if (from->left)
		return __tree_most_right(from->left);
	struct shl_rbt_node* parent = shl_rbt_parent(from);
	while (parent && parent->right != from){
		from = parent;
		parent = shl_rbt_parent(parent);
	}
	return parent;
}

static inline struct shl_rbt_node* __shl_rbt_get_replacer(
		struct shl_rbt_node* node)
{
	/* first repl check must be always left first */
	if (node->left){
		return __tree_most_right(node->left);
	}
	else if (node->right){
		return __tree_most_left(node->right);
	}
	return NULL;
}

/* replace instance in node's parent by new, 
 * then sets node's parent ptr to NULL*/
static inline void __shl_rbt_repl_instance(
		struct shl_rbt_node* node,
		struct shl_rbt_node* new)
{
	struct shl_rbt_node *parent = shl_rbt_parent(node);
	if (!parent)
		return;
	if (parent->right == node)
		parent->right = new;
	else if (parent->left == node)
		parent->left = new;

}

static inline struct shl_rbt_node* __shl_rbt_sibling(
		struct shl_rbt_node* from)
{
	if (!from->parent)
		return NULL;
	if (from->parent->left == from){
		return from->parent->right;
	}
	return from->parent->left;
}

static inline int __shl_rbt_get_child_cnt(
		struct shl_rbt_node* node)
{
	int cnt = 0;
	if (node->left)
		cnt++;
	if (node->right)
		cnt++;
	return cnt;
}

static inline void __shl_rbt_bin_remove(
		struct shl_rbt_node** root,
		struct shl_rbt_node** node,
		struct shl_rbt_node** repl_parent,
		struct shl_rbt_node** repl_sibling,
		unsigned int* repl_ncnt)
{
	/* validation is on shl_rbt_remove_node, this function is internal, so 
	 * do NOT use it anywhere else ;) */
	*repl_ncnt = 0;
	int reset_root = 0;
	if (*node == *root)
		reset_root = 1;
	struct shl_rbt_node
		*node_parent = shl_rbt_parent(*node),
		*replacer = __shl_rbt_get_replacer(*node);

	int set_after = 0;
	if (((*node)->left == replacer || (*node)->right == replacer) && 
			__shl_rbt_get_child_cnt(*node) == 2){
		set_after = 1;
	}
	*repl_parent = shl_rbt_parent(replacer);

	if (*repl_parent){
		*repl_sibling = __shl_rbt_sibling(replacer);
		if ((replacer)->left){
			(replacer->left->parent) = (*repl_parent);
			if ((*repl_parent)->right == replacer){
				(*repl_parent)->right = (replacer)->left;
			}
			else
				(*repl_parent)->left = (replacer)->left;
		}
		else if ((replacer)->right){
			(replacer->right->parent) = (*repl_parent);
			if ((*repl_parent)->right == replacer){
				(*repl_parent)->right = (replacer)->right;
			}
			else
				(*repl_parent)->left = (replacer)->right;
		}
	}
	else
		*repl_sibling = NULL;
	if (replacer){
		*repl_ncnt = __shl_rbt_get_child_cnt(replacer);
		__shl_rbt_repl_instance(replacer, NULL);
		replacer->parent = node_parent;
		
		if ((*node)->left){
			replacer->left = (*node)->left;
			replacer->left->parent = replacer;
		}
		if ((*node)->right){
			replacer->right = (*node)->right;
			replacer->right->parent = replacer;
		}
	}
	
	__shl_rbt_repl_instance(*node, replacer);
	(*node)->left = (*node)->right = (*node)->parent = NULL;
	if (set_after)
		*repl_parent = (*repl_sibling)->parent;
	*node = replacer;
	if (reset_root)
		*root = *node;
}

static inline void __shl_rbt_fix_dblack(
		struct shl_rbt_node** parent,
		struct shl_rbt_node** sibling,
		struct shl_rbt_node** root)
{
recheck: ;
	int s_is_left = ((*parent)->left == *sibling);
	if (is_black(*sibling)){
		/* if one of child of s is red */
		if (is_red((*sibling)->left) || is_red((*sibling)->right)){
			/* rr */
			if (!s_is_left && is_red((*sibling)->right)){
				int old_clr = (*parent)->color;
				int set_after = 0;
				if (*parent == *root)
					set_after = 1;
				__shl_rbt_turn_left(parent);
				if (set_after)
					*root = *parent;

				(*parent)->color = old_clr;
				(*parent)->right->color = RBT_BLACK;
				if ((*parent)->left)
					(*parent)->left->color = RBT_BLACK;
			}
			/* ll */
			else if (s_is_left && is_red((*sibling)->left)){
				int old_clr = (*parent)->color;
				int set_after = 0;
				if (*parent == *root)
					set_after = 1;
				__shl_rbt_turn_right(parent);
				if (set_after)
					*root = *parent;
			
				(*parent)->color = old_clr;
				(*parent)->left->color = RBT_BLACK;
				if ((*parent)->right)
					(*parent)->right->color = RBT_BLACK;

			}
			/* rl */
			else if (!s_is_left && is_red((*sibling)->left)){
				int tmp = (*sibling)->left->color;
				(*sibling)->left->color = (*sibling)->color;
				(*sibling)->color = tmp;
				int set_after = 0;
				if (*sibling == *root)
					set_after = 1;
				__shl_rbt_turn_right(sibling);
				if (set_after)
					*root = *sibling;
				goto recheck;

			}
			/* lr */
			else if (s_is_left && is_red((*sibling)->right)){
				int tmp = (*sibling)->right->color;
				(*sibling)->right->color = (*sibling)->color;
				(*sibling)->color = tmp;
				int set_after = 0;
				if (*sibling == *root)
					set_after = 1;
				__shl_rbt_turn_left(sibling);
				if (set_after)
					*root = *sibling;
				goto recheck;
			}
		}
		else{
			/* all child of sibling are black, so as sibling */
			if (is_black(*parent)){
				struct shl_rbt_node
					*nparent = (*parent)->parent,
					*nsibling = NULL;

				if(*sibling)
					(*sibling)->color = RBT_RED;
				if (!nparent){
					return;
				}
				if (nparent->left == *parent)
					nsibling = nparent->right;
				else
					nsibling = nparent->left;

				*parent = nparent;
				*sibling = nsibling;
				goto recheck;
			}
			else{
				if (s_is_left)
					(*parent)->left->color = RBT_RED;
				else
					(*parent)->right->color = RBT_RED;
				(*parent)->color = RBT_BLACK;
			}
		}
	}
	else {
		if (!s_is_left){
			int set_after = 0;
			if (*parent == *root)
				set_after = 1;
			struct shl_rbt_node* old_parent = *parent,
				*new_sibling = NULL;
			__shl_rbt_turn_left(parent);
			if (set_after)
				*root = *parent;
			new_sibling = old_parent->right;
			(*parent)->left->color = RBT_RED;
			(*parent)->color = RBT_BLACK;
			*parent = old_parent;
			*sibling = new_sibling;
			goto recheck;
		}
		else if (s_is_left){
			int set_after = 0;
			if (*parent == *root)
				set_after = 1;
			struct shl_rbt_node* old_parent = *parent,
				*new_sibling = NULL;
			__shl_rbt_turn_right(parent);
			if (set_after)
				*root = *parent;
			new_sibling = old_parent->left;
			(*parent)->right->color = RBT_RED;
			(*parent)->color = RBT_BLACK;
			*parent = old_parent;
			*sibling = new_sibling;
			goto recheck;

		}
	}
}

static inline void shl_rbt_remove_node(
		struct shl_rbt_node** root,
		struct shl_rbt_node* node)
{
	if (!root || !*root || !node)
		return;
	int dnode_color = (node)->color,
	    child_cnt = __shl_rbt_get_child_cnt(node),
	    /* color of node that was deleted. */
	    repl_old_color;

	/* those are default values of parent, sibling, in case that
	 * *node after bst insert might be NULL */
	struct shl_rbt_node *parent = (node)->parent,
			*sibling = __shl_rbt_sibling(node),
			*repl_parent = NULL,
			*repl_sibling = NULL;
	unsigned int repl_ncnt;
	if (!parent){
		if (child_cnt == 0){
			*root = NULL;
			return;
		}
	}
	__shl_rbt_bin_remove(root, &node, &repl_parent, &repl_sibling, &repl_ncnt);
	if (node){
		repl_old_color = (node)->color;
		(node)->color = dnode_color;
	}
	else
		repl_old_color = RBT_BLACK;

	if (child_cnt == 0){
		/* repl_old_color here is 100% black */
		if (dnode_color == RBT_BLACK){
			int set_after = 0;
			if (!parent->parent)
				set_after = 1;
			__shl_rbt_fix_dblack(&parent, &sibling, root);
			if (set_after)
				*root = parent;
		}
	}
	else if (child_cnt == 1){
		if (repl_old_color == RBT_BLACK || dnode_color == RBT_BLACK){
			(node)->color = RBT_BLACK;
		}
	}
	else if (child_cnt == 2){
		if (dnode_color == RBT_BLACK &&	repl_ncnt == 1){
			if (repl_parent->left == repl_sibling){
				repl_parent->right->color = RBT_BLACK;
			}
			else
				repl_parent->left->color = RBT_BLACK;
		}	
		else if (repl_old_color == RBT_RED){

		}
		else if (dnode_color == RBT_BLACK || repl_old_color == RBT_BLACK){
			__shl_rbt_fix_dblack(&repl_parent, &repl_sibling, root);
		}
	}
	if (*root)
		(*root)->color = RBT_BLACK;
}

/* SECTION BELOW MAKES ABSTRACTIONS OVER NODES */
#ifdef SHL_RBT_ABSTRACTIONS

#ifndef SHL_RBT_OWNMALLOC
#include <stdlib.h>
#endif /* SHL_RBT_OWNMALLOC */

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

static inline shl_tree_node_t* shl_tree_new_node_full(
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
	shl_rbt_node_t* found = shl_rbt_find_node_full(
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
	shl_rbt_node_t* root = NULL;

	shl_tree_node_t* newnode = shl_tree_find(tree, key);
	if (!newnode){
		newnode = shl_tree_new_node_full(key, data);
		if (!newnode)
			return NULL;
	}
	else{
		newnode->data = data;
		return newnode;
	}
	if (!tree->root){
		shl_rbt_insert_node_full(&root, &(newnode->rbt_node),
				shl_tree_node_cmp_full, tree);
		tree->root = newnode;
		return newnode;
	}
	root = &(tree->root->rbt_node);
	shl_rbt_insert_node_full(&root,
			&(newnode->rbt_node), shl_tree_node_cmp_full, tree);
	tree->root = shl_get_entry(root, shl_tree_node_t, rbt_node);
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

static inline void shl_tree_remove_full(
		shl_tree_t* tree,
		shl_tree_node_t* node,
		void* user_data)
{
	shl_tree_unlink(tree, node);
	if (tree->key_destroy)
		tree->key_destroy(tree, node->key, user_data);
	if (tree->data_destroy)
		tree->data_destroy(tree, node->data, user_data);
	free(node);
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

#endif /* SHL_RBT_ABSTRACTIONS */

#endif /* _H_SHL_RB_TREE_H */
