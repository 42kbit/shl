#include <stdio.h>
#include "../../shl_list.h"

struct data {
	int data;
	struct shl_list_node list;
};

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
	shl_list_insert(list, pdata1);
	shl_list_insert(list, pdata2);
	shl_list_insert(list, pdata3);
	shl_list_insert(list, pdata4);
    /*
     * Uncomment following line to witness linux-list double insertion bug.
     * However, note that we may not initialize nodes.
	shl_list_insert(list, pdata4);
    */

	if (shl_list_traverse(list, trav_func, NULL) != 0)
		printf("List traverse error\n");
}
