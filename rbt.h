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

#define is_red(node) (node->color == RBT_RED)

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
		struct rbt_node* root,
		struct rbt_node** node,
		rbt_cmp_node cmp)
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
		struct rbt_node* root,
		struct rbt_node** node,
		rbt_cmp_node cmp)
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
				__rbt_turn_left(*root, &parent, ncmp);
				node = parent->left;
			}
			else if(parent->left == node && !turn_right){
				__rbt_turn_right(*root, &parent, ncmp);
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
			turn_right? __rbt_turn_right(*root, &gparent, ncmp):
				__rbt_turn_left(*root, &gparent, ncmp);
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
	if (node->right){
		return __tree_most_left(node->right);
	}
	else if (node->left){
		return __tree_most_right(node->left);
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
	if (parent->left == node)
		parent->left = new;
	else if (parent->right == node)
		parent->right = new;
}

static inline void __rbt_bin_remove(
		struct rbt_node* root,
		struct rbt_node** node)
{
	/* validation is on rbt_remove, this function is internal, so 
	 * do NOT use it anywhere else ;) */
	struct rbt_node
		*node_parent = rbt_parent(*node),
		*replacer = __rbt_get_replacer(*node);
	if (replacer){
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
	*node = replacer;
}

static inline void rbt_remove(
		struct rbt_node* root,
		struct rbt_node** node)
{
	if (!root || !node || !*node)
		return;
	int dnode_color = (*node)->color;
	__rbt_bin_remove(root, node);
	printf("%d\n", dnode_color);
}

#endif /* _H_RB_TREE_H */
