#include <stdio.h>

#define SHL_RBT_NOABSTRACTIONS
#include "../../../shl_rbt.h"

struct obj {
	unsigned int data;
	struct shl_rbt_node rbt_node;
};

/* similar to obj_cmp_node, but takes key */
static inline int obj_cmp_key(struct shl_rbt_node* node, const void* key,
		const void* _){
	struct obj *ent;
	ent = shl_get_entry(node, struct obj, rbt_node);
	return ent->data - *(unsigned int*)key;
}

static inline int obj_cmp_node(
		struct shl_rbt_node* node0,
		struct shl_rbt_node* node1,
		const void* _)
{
	return obj_cmp_key(node0, shl_get_entry(node1, struct obj, rbt_node),
			NULL);
}

#define arr_size(name) \
	sizeof(name) / sizeof(*name)

int main(void){
	struct obj root_obj;
	struct shl_rbt_node* root = &(root_obj.rbt_node);
	root_obj.data = 8;
	shl_rbt_init_node(root);

	unsigned int ins_vals[] = {3,1,6,4,7,10,14,13};
	struct obj objs[arr_size(ins_vals)];

	for(int i = 0; i < arr_size(ins_vals); i++){
		objs[i].data = ins_vals[i];
		shl_rbt_insert_node(&root, &(objs[i].rbt_node), 
				obj_cmp_node);
	}
	
	unsigned int element = 6;
	struct shl_rbt_node* node = shl_rbt_find_node_full(root, &element, obj_cmp_key, NULL);
	if (node)
		printf("node located in addr:%p, data:%u\n", 
				(void*)shl_get_entry(node, struct obj, rbt_node),
				shl_get_entry(node, struct obj, rbt_node)->data);
	else
		printf("node is not present\n");
	return 0;
}
