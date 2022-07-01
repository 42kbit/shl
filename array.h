#ifndef _H_ARRAY_H
#define _H_ARRAY_H

#include <string.h>

static inline int __arr_get_ceil(
		void* arr,
		unsigned int width,
		unsigned int low,
		unsigned int high,
		void* element,
		int (*cmp)(const void*, const void*))
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
			return __arr_get_ceil(
					arr, 
					width,
					mid+1,
					high,
					element,
					cmp);
	}
	if (mid - 1 >= low && cmp(element, ptr+(mid-1)*width) > 0)
		return mid;
	else
		return __arr_get_ceil(
				arr,
				width,
				low,
				mid-1,
				element,
				cmp);
}

static inline int __arr_bin_insert(
		void* arr,
		unsigned int size,
		unsigned int width,
		void* element,
		int (*cmp)(const void*, const void*)
		)
{
	int index = __arr_get_ceil(arr, width, 0, size - 1, element, cmp);
	memmove(arr+(index+1)*width, arr+(index)*width, (size-index)*width);
	memcpy(arr+index*width, element, width);
	return index;
}

#define arr_bin_insert_auto(ptr, size, element, cmp) \
	__arr_bin_insert(ptr, size, sizeof(*element), element, cmp)

#endif /* _H_ARRAY_H */
