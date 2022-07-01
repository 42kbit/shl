#include <stdio.h>
#include "../array.h"

#ifndef MAX
#define MAX(a,b) (a > b?a:b)
#endif
int int_cmp(const void* i0, const void* i1){
	int v0 = *((const int*)i0);
	int v1 = *((const int*)i1);
	return (v0 - v1);
}

int main(void){
	int arr[9] = {11, 22, 33, 44, 55, 66, 77, 88, 99};
	int element = 23;
	int ind = __arr_get_ceil(
			arr, 
			sizeof(*arr),
			0,
			8,
			&element,
			int_cmp);
	printf("ceil of %d, is at %u\n", element, ind);
	arr_bin_insert_auto(arr, 9, &element, int_cmp);
	for(int i = 0; i < 9; i++){
		printf("%d\n", arr[i]);
	}
	return 0;
}
