#include <stdio.h>
#include "../../../shl_rbt.h"

struct obj {
	unsigned int data;
	struct shl_rbt_node rbt_node;
};

int obj_cmp_node(struct shl_rbt_node* node0, struct shl_rbt_node* node1){
	struct obj *obj0, *obj1;
	obj0 = shl_get_entry(node0, struct obj, rbt_node);
	obj1 = shl_get_entry(node1, struct obj, rbt_node);
	return obj0->data - obj1->data;
}

int main(void){
	struct obj obj0;
	obj0.data = 55;
	struct obj obj1;
	obj1.data = 44;

	struct shl_rbt_node* root = NULL;
	struct shl_rbt_node* obj0_node = &(obj0.rbt_node);
	struct shl_rbt_node* obj1_node = &(obj1.rbt_node);

	shl_rbt_insert_node(&root, obj0_node, obj_cmp_node);
	shl_rbt_insert_node(&root, obj1_node, obj_cmp_node);
	printf("%u is obj data!\n", 
			shl_get_entry(root, struct obj, rbt_node)->data);
	printf("%u is obj data!\n", 
			shl_get_entry(root->left, struct obj, rbt_node)->data);
	return 0;
}
