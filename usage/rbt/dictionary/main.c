/* quite unefficent i guess, but im too dumb for implementing the
 * hashing algorithm */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

struct obj_data{
	int value;
	/*
	 * other data
	 */
};

struct obj_dict{
	struct obj_data data;
	/* dict_node contains key, with tree node (no hashmap, cuz example
	 * for red-black tree implementation) */
	struct dict_node dict_node;
};

void obj_dict_set_val(struct dict_node* node, void* value){
	struct obj_dict *entry = shl_get_entry(node, struct obj_dict,
			dict_node);
	entry->data.value = *(int*)value;
}

#define arr_size(x) \
	sizeof(x) / sizeof(x[0])

int main(void){
	struct dict_node* root = NULL;
	char* strings[] = {"a", "b", "c", "d", "e",
	"f", "g", "h", "hello"};
	struct obj_dict obj_dicts[arr_size(strings)];
	for(int i = 0; i < arr_size(strings); i++){
		strncpy(obj_dicts[i].dict_node.key,
				strings[i], DICT_NODE_DLEN);
		/* so each associated name will have index in strings
		 * array */
		dict_insert(&root, &(obj_dicts[i].dict_node), &i,
				obj_dict_set_val);
	}

	dict_remove(&root, "d");
	dict_remove(&root, "c");
	struct dict_node* found = dict_get(root, "a"); /* a is 0, ok! */
	if (found)
		/* upcast struct dict_node* to struct obj_dict* */
		printf("%d\n", shl_get_entry(found, struct obj_dict, 
					dict_node)->data.value);
	found = dict_get(root, "d"); /* d was deleted, will return NULL! */
	if (!found){
		printf("d was not found!\n");
	}
	found = dict_get(root, "hello"); /* hello index is 8 */
	if (found){
		printf("%d\n", shl_get_entry(found, struct obj_dict, 
					dict_node)->data.value);
	}
	return 0;
}
