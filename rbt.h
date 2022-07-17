#ifndef _H_RB_TREE_H
#define _H_RB_TREE_H

/* one RED-BLACK TREE a day, keeps bin_tree.h away... */

#ifndef NULL
#define NULL ((void*)0)
#endif

#define get_entry(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

#define RBT_BLACK 0
#define RBT_RED 1

struct rbt_node{
	struct rbt_node *right, *left, *parent;
	char color;
};

#define is_red(node) ((node) && ((node)->color == RBT_RED))
#define is_black(node) (!(node) || ((node)->color == RBT_BLACK))

#define __define_tree_most(dir) 			\
static inline struct rbt_node* __tree_most_##dir( 	\
		struct rbt_node* root){ 		\
	struct rbt_node* iter = root; 			\
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

static inline void rbt_init_node(
		struct rbt_node* node)
{
	node->left = node->right = node->parent = NULL;
	node->color = RBT_RED;
}


typedef int(*rbt_cmp_key)(struct rbt_node*, const void*);
typedef int(*rbt_cmp_node)(struct rbt_node*, struct rbt_node*);

static inline struct rbt_node* rbt_find(
		struct rbt_node* root,
		const void* key,
		rbt_cmp_key kcmp)
{
	if (!root)
		return NULL;
	int res = kcmp(root, key);
	if (res == 0)
		return root;
	else if (res > 0)
		return rbt_find(root->left, key, kcmp);
	return rbt_find(root->right, key, kcmp);
}

static inline struct rbt_node* rbt_parent(
		struct rbt_node* child)
{
	if (!child)
		return NULL;
	return child->parent;
}

static inline void __rbt_turn_left(
		struct rbt_node** node)
{
	if (!((*node)->right))
		return;
	struct rbt_node *n_node = (*node)->right,
			*n_right = n_node->left,
			*parent = rbt_parent(*node);
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

static inline void __rbt_turn_right(
		struct rbt_node** node)
{
	if (!((*node)->left))
		return;
	struct rbt_node *n_node = (*node)->left,
			*n_left = n_node->right,
			*parent = rbt_parent(*node);
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

static inline struct rbt_node* __rbt_add(
		struct rbt_node* root,
		struct rbt_node* node,
		rbt_cmp_node ncmp)
{
	int cmp = ncmp(root, node);
	if (cmp < 0){
		if (!root->right){
			node->parent = root;
			return (root->right = node);
		}
		return __rbt_add(root->right, node, ncmp);
	}
	else if (cmp > 0){
		if (!root->left){
			node->parent = root;
			return (root->left = node);
		}
		return __rbt_add(root->left, node, ncmp);
	}
	return NULL;
}

static inline void __rbt_push_blackness(
		struct rbt_node* from)
{
	/* no validation for now */
	from->color = RBT_RED;
	if (from->left)
		from->left->color = RBT_BLACK;
	if (from->right)
		from->right->color = RBT_BLACK;
}

/* works like magic but i'm proud of it */
static inline void __rbt_added_relocate(
		struct rbt_node** root,
		struct rbt_node* node,
		rbt_cmp_node ncmp)
{
	struct rbt_node *parent = rbt_parent(node),
			*gparent = rbt_parent(parent),
			*sibling = NULL,
			*uncle = NULL;
	int turn_right = 0;
	if (!gparent || !parent)
		return;
	turn_right = (gparent->left == parent);
	uncle = turn_right? gparent->right : gparent->left;
	sibling = parent->left == node? parent->right : parent->left;
	if (is_red(parent) && is_red(node)){
		/* maybe !is_red is not needed, but idk */
		if (!uncle || !is_red(uncle)){
			if (parent->right == node && turn_right){
				__rbt_turn_left(&parent);
				node = parent->left;
			}
			else if(parent->left == node && !turn_right){
				__rbt_turn_right(&parent);
				node = parent->right;
			}
		}
		if (uncle && is_red(uncle)){
			__rbt_push_blackness(gparent);
			__rbt_added_relocate(root, gparent, ncmp);
		}
		else if (!uncle || !is_red(uncle)){
			gparent->color = RBT_RED;
			parent->color = RBT_BLACK;
			node->color = RBT_RED;
			/* set new root, if grandparent is equal to root */
			int set_after = 0;
			if (*root == gparent)
				set_after = 1;
			turn_right? __rbt_turn_right(&gparent):
				__rbt_turn_left(&gparent);
			if (set_after)
				*root = gparent;
		}
		
	}
	(*root)->color = RBT_BLACK;
}

/* add + validate to make binary search tree
 * red and black tree. */
static inline void rbt_insert(
		struct rbt_node** root,
		struct rbt_node* node,
		rbt_cmp_node ncmp)
{
	if (!*root){
		node->color = RBT_BLACK;
		*root = node;
		return;
	}
	struct rbt_node* added = __rbt_add(*root, node, ncmp);
	__rbt_added_relocate(root, added, ncmp);
}

static inline struct rbt_node* rbt_next(
		struct rbt_node* root,
		struct rbt_node* from,
		rbt_cmp_node cmp)
{
	if (!root)
		return NULL;
	if (!from)
		return __tree_most_left(root);
	if (from->right)
		return __tree_most_left(from->right);
	struct rbt_node* parent = rbt_parent(from);
	while (parent && parent->left != from){
		from = parent;
		parent = rbt_parent(parent);
	}
	return parent;
}

static inline struct rbt_node* rbt_prev(
		struct rbt_node* root,
		struct rbt_node* from,
		rbt_cmp_node cmp)
{
	if (!root)
		return NULL;
	if (!from)
		return __tree_most_right(root);
	if (from->right)
		return __tree_most_right(from->left);
	struct rbt_node* parent = rbt_parent(from);
	while (parent && parent->right != from){
		from = parent;
		parent = rbt_parent(parent);
	}
	return parent;
}

static inline struct rbt_node* __rbt_get_replacer(
		struct rbt_node* node)
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
static inline void __rbt_repl_instance(
		struct rbt_node* node,
		struct rbt_node* new)
{
	struct rbt_node *parent = rbt_parent(node);
	if (!parent)
		return;
	if (parent->right == node)
		parent->right = new;
	else if (parent->left == node)
		parent->left = new;

}

static inline struct rbt_node* __rbt_sibling(
		struct rbt_node* from)
{
	if (!from->parent)
		return NULL;
	if (from->parent->left == from){
		return from->parent->right;
	}
	return from->parent->left;
}

static inline int __rbt_get_child_cnt(
		struct rbt_node* node)
{
	int cnt = 0;
	if (node->left)
		cnt++;
	if (node->right)
		cnt++;
	return cnt;
}

static inline void __rbt_bin_remove(
		struct rbt_node** root,
		struct rbt_node** node,
		struct rbt_node** repl_parent,
		struct rbt_node** repl_sibling,
		unsigned int* repl_ncnt)
{
	/* validation is on rbt_remove, this function is internal, so 
	 * do NOT use it anywhere else ;) */
	*repl_ncnt = 0;
	int reset_root = 0;
	if (*node == *root)
		reset_root = 1;
	struct rbt_node
		*node_parent = rbt_parent(*node),
		*replacer = __rbt_get_replacer(*node);

	int set_after = 0;
	if (((*node)->left == replacer || (*node)->right == replacer) && 
			__rbt_get_child_cnt(*node) == 2){
		set_after = 1;
	}
	*repl_parent = rbt_parent(replacer);

	if (*repl_parent){
		*repl_sibling = __rbt_sibling(replacer);
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
		*repl_ncnt = __rbt_get_child_cnt(replacer);
		__rbt_repl_instance(replacer, NULL);
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
	
	__rbt_repl_instance(*node, replacer);
	(*node)->left = (*node)->right = (*node)->parent = NULL;
	if (set_after)
		*repl_parent = (*repl_sibling)->parent;
	*node = replacer;
	if (reset_root)
		*root = *node;
}

static inline void __rbt_fix_dblack(
		struct rbt_node** parent,
		struct rbt_node** sibling,
		struct rbt_node** root)
{
recheck:
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
				__rbt_turn_left(parent);
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
				__rbt_turn_right(parent);
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
				__rbt_turn_right(sibling);
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
				__rbt_turn_left(sibling);
				if (set_after)
					*root = *sibling;
				goto recheck;
			}
		}
		else{
			/* all child of sibling are black, so as sibling */
			if (is_black(*parent)){
				struct rbt_node
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
				__rbt_fix_dblack(&nparent, &nsibling, root);
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
			struct rbt_node* old_parent = *parent,
				*new_sibling = NULL;
			__rbt_turn_left(parent);
			if (set_after)
				*root = *parent;
			new_sibling = old_parent->right;
			(*parent)->left->color = RBT_RED;
			(*parent)->color = RBT_BLACK;
			__rbt_fix_dblack(&old_parent, &new_sibling, root);
			
		}
		else if (s_is_left){
			int set_after = 0;
			if (*parent == *root)
				set_after = 1;
			struct rbt_node* old_parent = *parent,
				*new_sibling = NULL;
			__rbt_turn_right(parent);
			if (set_after)
				*root = *parent;
			new_sibling = old_parent->left;
			(*parent)->right->color = RBT_RED;
			(*parent)->color = RBT_BLACK;
			__rbt_fix_dblack(&old_parent, &new_sibling, root);
		}
	}
}

static inline void rbt_remove(
		struct rbt_node** root,
		struct rbt_node** node)
{
	if (!root || !*root || !node || !*node)
		return;
	int dnode_color = (*node)->color,
	    child_cnt = __rbt_get_child_cnt(*node),
	    /* color of node that was deleted. */
	    repl_old_color,
	    repl_was_left;

	/* those are default values of parent, sibling, in case that
	 * *node after bst insert might be NULL */
	struct rbt_node *parent = (*node)->parent,
			*sibling = __rbt_sibling(*node),
			*old_node = (*node),
			*repl_parent = NULL,
			*repl_sibling = NULL;
	unsigned int repl_ncnt;

	repl_was_left = !!((*node)->left);
	if (!parent){
		if (child_cnt == 0){
			*root = NULL;
			*node = NULL;
			return;
		}
	}
	__rbt_bin_remove(root, node, &repl_parent, &repl_sibling, &repl_ncnt);
	if (*node){
		repl_old_color = (*node)->color;
		(*node)->color = dnode_color;
	}
	else
		repl_old_color = RBT_BLACK;

	if (child_cnt == 0){
		/* repl_old_color here is 100% black */
		if (dnode_color == RBT_BLACK){
			int set_after = 0;
			if (!parent->parent)
				set_after = 1;
			__rbt_fix_dblack(&parent, &sibling, root);
			if (set_after)
				*root = parent;
		}
	}
	else if (child_cnt == 1){
		if (repl_old_color == RBT_BLACK || dnode_color == RBT_BLACK){
			(*node)->color = RBT_BLACK;
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
			__rbt_fix_dblack(&repl_parent, &repl_sibling, root);
		}
	}
	if (*root)
		(*root)->color = RBT_BLACK;
}

#endif /* _H_RB_TREE_H */
