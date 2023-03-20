#ifndef __H_USAGE_ELF_LINKER_SRC_INCLUDE_LIB_IO_H
#define __H_USAGE_ELF_LINKER_SRC_INCLUDE_LIB_IO_H

#include <stdarg.h>
#include <stddef.h>

void putc	(char c);

int vsnprintf	(char * buf, size_t size, const char * fmt, va_list args);
int vsprintf	(char * buf, const char * fmt, va_list args);
int vprintf	(const char * fmt, va_list args);
int sprintf	(char * buf, const char * fmt, ...);
int printf	(const char * fmt, ...);

/* Tell msg, then suicide */
void panic	(const char* fmt, ...);

#endif /* __H_USAGE_ELF_LINKER_SRC_INCLUDE_LIB_IO_H */
