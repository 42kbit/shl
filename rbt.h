#ifndef _H_RB_TREE_H
#define _H_RB_TREE_H

/* one RED-BLACK TREE a day, keeps bin_tree.h away... */

#define RBT_BLACK 0
#define RBT_RED 1
struct rbt_node{
	struct rbt_node *right, *left;
	char rbt_color;
};

static inline void rbt_rotate_left(
		struct tree_node** node)
{
	if (!((*node)->right))
		return;
	struct tree_node *n_node = (*node)->right,
			 *n_right = n_node->left;
	(*node)->right = n_right;
	n_node->left = *node;
	*node = n_node;
}

static inline void rbt_rotate_right(
		struct tree_node** node)
{
	if (!((*node)->left))
		return;
	struct tree_node *n_node = (*node)->left,
			 *n_left = n_node->right;
	(*node)->left = n_left;
	n_node->right = *node;
	*node = n_node;
}

#endif /* _H_RB_TREE_H */
