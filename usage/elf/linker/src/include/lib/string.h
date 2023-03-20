#ifndef __H_USAGE_ELF_LINKER_SRC_INCLUDE_LIB_STRING_H
#define __H_USAGE_ELF_LINKER_SRC_INCLUDE_LIB_STRING_H

#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>

void * memcpy (void * dst, const void * src, size_t size);
void * memset (void * dst, int c, size_t size);
size_t strlen (const char * str);

void __strncat_len(char* s1, const char * s2, size_t s,
		size_t * __s1_len, size_t * __was_written);

char * strncat (char * s1, const char * s2, size_t s);

char * strcat (char * s1, const char * s2);

int strcmp (const char* s1, const char* s2);

#endif /* __H_USAGE_ELF_LINKER_SRC_INCLUDE_LIB_STRING_H */
