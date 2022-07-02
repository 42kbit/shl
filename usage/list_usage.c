#include <stdio.h>
#include "../list.h"

struct data {
	int data;
	struct list_node list;
};


int sdata_by_data (struct list_node* node, void* data){
	return get_entry(node, struct data, list)->data == *((int*)data);
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

	struct list_node head,
			 *pdata1 = &(data1.list),
			 *pdata2 = &(data2.list),
			 *pdata3 = &(data3.list),
			 *pdata4 = &(data4.list),
			 *list = &head;

	init_list_node(list);
	list_add(list, pdata1);
	list_add(list, pdata2);
	list_add(list, pdata3);
	list_add(list, pdata4);
	list_move(list, pdata3);
	int val = 22;
	list_remove(list_find(list, (void*)&val, sdata_by_data));
	
	struct list_node* cur;
	struct data* ent;
	list_for_each_entry_prev(list, cur, ent, struct data, list){
		printf("%d\n", ent->data);
	}

	list_for_each_entry_prev_auto(list, cur, ent, list){
		printf("%d\n", ent->data);
	}

}
