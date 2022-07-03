#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../array.h"

#ifndef MAX
#define MAX(a,b) (a > b?a:b)
#endif
int int_cmp(const void* i0, const void* i1){
	int v0 = *((const int*)i0);
	int v1 = *((const int*)i1);
	return (v0 - v1);
}

#define ARR_SIZE 999999

int main(void){
	srand((unsigned long)time(NULL));
	int *arr = (int*)malloc(ARR_SIZE * sizeof(int));
	memset(arr, 0, ARR_SIZE * sizeof(int));
	for (int i = 0; i < ARR_SIZE; i++){
		*(char*)((char*)arr+i*sizeof(int)+3) = (char)0x80;
	}
	for (int i = 0; i < ARR_SIZE; i++){
		int val = i;
		/*arr_bin_insert_auto(arr, ARR_SIZE, &val, int_cmp);*/
		__arr_bin_insert(
				arr,
				ARR_SIZE,
				4,
				&val,
				int_cmp
				);
	}
	for (int i = 0; i < ARR_SIZE; i++){
		printf("%d\n", arr[i]);
	}
	free(arr);
	return 0;
}
