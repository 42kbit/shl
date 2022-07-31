#ifndef _H_SHL_ARR_H
#define _H_SHL_ARR_H

#include <string.h>

typedef unsigned long ul_t;

static inline int __shl_arr_insert(void* arr, ul_t width, ul_t to, 
		void* element)
{
	memcpy((char*)arr+to*width, element, width); 
}

/* im sorry, this is too magic */
static inline int __shl_arr_get_ceil(
		void* arr,
		ul_t width,
		ul_t low,
		ul_t high,
		void* element,
		int (*cmp)(
			const void*,
			const void*))
{
	char* ptr = (char*)arr;
	/* if lower than first element, return index of the first element */
	if (cmp(element, ptr+width*low) <= 0){
		return low;
	}
	/* if higher than last element, return index of the last element */
	if (cmp(element, ptr+width*high) > 0)
		return high;
	unsigned int mid = (low + high) / 2;
	/* if equals to middle element, return middle element index */
	if (cmp(element, ptr+width*mid) == 0)
		return mid;
	else if (cmp(element, ptr+width*mid) > 0){
		/* search right */
		if (mid + 1 <= high && cmp(element, ptr+(mid+1)*width) <= 0)
			return mid + 1;
		else
			return __shl_arr_get_ceil(
					arr, 
					width,
					mid+1,
					high,
					element,
					cmp);
	}
	else {
		if (mid - 1 >= low && cmp(element, ptr+(mid-1)*width) > 0)
			return mid;
		else
			return __shl_arr_get_ceil(
					arr,
					width,
					low,
					mid-1,
					element,
					cmp);
	}
}

static inline int __shl_arr_bin_insert(
		void* arr,
		ul_t size,
		ul_t width,
		void* element,
		int (*cmp)(const void*, const void*)
		)
{
	int index = __shl_arr_get_ceil(arr, width, 0, size - 1, element, cmp);
	memmove((char*)arr+(index+1)*width, 
			(char*)arr+index*width, (size-index-1)*width);
	memcpy((char*)arr+index*width, element, width);
	return index;
}

#define arr_bin_insert_auto(ptr, size, element, cmp) \
	__shl_arr_bin_insert(ptr, size, sizeof(*element), element, cmp)

#define arr_insert(ptr, index, element) \
	__shl_arr_insert(ptr, sizeof(*element), index, element)

#endif /* _H_SHL_ARR_H */
