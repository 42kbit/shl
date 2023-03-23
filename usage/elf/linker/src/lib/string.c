#include <lib/string.h>

void * memcpy (void * dst, const void * src, size_t size){
	for (uint32_t i = 0; i < size; i++){
		((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
	}
	return dst;
}

void * memset (void * dst, int c, size_t size){
	for (uint32_t i = 0; i < size; i++){
		((uint8_t*)dst)[i] = (uint8_t)c;
	}
	return dst;
}

size_t strlen (const char * str){
	size_t i;
	for (i = 0; str[i] != '\0'; i++);
	return i;
}

void __strncat_len(char* s1, const char * s2, size_t s,
		size_t * __s1_len, size_t * __was_written)
{
	size_t s1_len = strlen(s1);
	int32_t i;
	for (i = 0; s > 0 && s2[i] != '\0'; i++, s--){
		s1[s1_len + i] = s2[i];
	}

	if (__s1_len) 		*__s1_len = s1_len;
	if (__was_written) 	*__was_written = (size_t)i;
}

char * strncat (char * s1, const char * s2, size_t s){
	__strncat_len(s1, s2, s, NULL, NULL);
	return s1;
}

char * strcat (char * s1, const char * s2){
	return strncat(s1, s2, INT_MAX);
}

int strcmp (const char* s1, const char* s2){
	int c;
	while ((c =*(s2++) - *(s1++)) == 0 && *s1 && *s2);
	return c;
}