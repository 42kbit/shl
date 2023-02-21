#include <stdio.h>
#include "../../shl_list.h"

struct data {
	int data;
	struct shl_list_node list;
};

int sdata_by_data (
		struct shl_list_node* node,
		const void* data,
		void* _)
{
	return shl_get_entry(node, struct data, list)->data == *((int*)data);
}

int trav_func (struct shl_list_node* node, void* user_data){
	struct data* data = shl_get_entry(node, struct data, list);
	printf("%d\n", data->data);
	return 0;
}

int main(void){
	struct data data1;
	struct data data2;
	struct data data3;
	struct data data4;
	data1.data = 11;
	data2.data = 22;
	data3.data = 33;
	data4.data = 44;

	struct shl_list_node head,
			 *pdata1 = &(data1.list),
			 *pdata2 = &(data2.list),
			 *pdata3 = &(data3.list),
			 *pdata4 = &(data4.list),
			 *list = &head;

	shl_list_init_head(list);
	/*
	 * We MUST initialize nodes in order to shl_list_insert_safe to work.
	*/
	shl_list_init_node(pdata1);
	shl_list_init_node(pdata2);
	shl_list_init_node(pdata3);
	shl_list_init_node(pdata4);
	shl_list_insert_safe(list, pdata1);
	shl_list_insert_safe(list, pdata2);
	shl_list_insert_safe(list, pdata3);
	shl_list_insert_safe(list, pdata4);
	/* inserting node again, will return non-zero value,
	 * print error to demonstrate that it is (indeed) working.
	*/
	if (shl_list_insert_safe(list, pdata4) != 0){
		printf("Node Inserted again!\n");
	}
	
	/* SEEK AND DESTROY the element by data */
	int val = 22;
	shl_list_remove(shl_list_find(list, (void*)&val, sdata_by_data, NULL));
	
	/* 
	 * One way to traverse the tree. Unsafe if you are planning to free().
	 * With intentions to free() better use shl_list_traverse.
	*/
	struct shl_list_node* cur;
	struct data* ent;
	shl_list_for_each_entry_prev(list, cur, ent, struct data, list){
		printf("%d\n", ent->data);
	}

	/* Same but more clean*/
	shl_list_for_each_entry_prev_auto(list, cur, ent, list){
		printf("%d\n", ent->data);
	}
	
	/* 
	 * Another way of traversing the tree, using callback function.
	 * Safe way to free() list node in callback.
	*/
	if (shl_list_traverse(list, trav_func, NULL) != 0)
		printf("List traverse error\n");
}
