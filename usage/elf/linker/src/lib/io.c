#include <inttypes.h>
#include <limits.h>
#include <errno.h>

#include <lib/io.h>
#include <lib/string.h>
#include <lib/cmn.h>
#include <unistd.h>
#include <stdlib.h>

void putc	(char c){
	write (1, &c, 1);
}

#define PRINTF_MAX_LEN 128
int vsprintf	(char * dst, const char * fmt, va_list args){
	return vsnprintf (dst, PRINTF_MAX_LEN, fmt, args);
}

int sprintf	(char * dst, const char * fmt, ...){
	va_list args;
	va_start (args, fmt);

	int val = vsprintf(dst, fmt, args);

	va_end (args);
	return val;
}

int vprintf	(const char * fmt, va_list args){
	static char str[PRINTF_MAX_LEN] __attribute__((aligned(16)));
	memset(str, 0, PRINTF_MAX_LEN * sizeof(char));
	int written;
	written = vsnprintf (str, PRINTF_MAX_LEN, fmt, args);
	
	for (const char * i = str; *i != '\0'; i++){
		putc(*i);
	}
	return written;
}

static inline void __suicide (void) {
	exit (-1);
}

void panic	(const char* fmt, ...){
	va_list args;
	va_start (args, fmt);
	vprintf(fmt, args);
	va_end (args);
	__suicide ();
}

int printf	(const char * fmt, ...){
	va_list args;
	va_start (args, fmt);

	int val = vprintf(fmt, args);

	va_end (args);
	return val;
}

static inline void reverse_inplace(char * str, intmax_t len){
	char tmp;
	for (intmax_t i = 0; i < len/2; i++){
		tmp = str[i];
		str[i] = str[len-i-1];
		str[len-i-1] = tmp;
	}
}

static inline ptrdiff_t __psize (addr_t num){
	ptrdiff_t len = 0;
	while (num != 0){
		len++;
		num /= 16;
	}
	return len;
}

static inline ptrdiff_t utoa (uint32_t n, char * buf, uint8_t base, const char* digits){
	char * buf_iter = buf;
	int32_t digit;
	while (n != 0){
		digit = n % base;
		n /= base;

		*(buf_iter++) = digits[digit];
	}
	reverse_inplace(buf, strlen(buf));
	return buf_iter - buf;
}

int vsnprintf	(char * buf, size_t size, const char * fmt, va_list args)
{
	const char * fmt_ptr = fmt;
	char * buf_ptr = buf, c;
	ptrdiff_t diff;

	static const char * digits = "0123456789abcdef";

	while ((c = *(fmt_ptr++)) && size-- > 0){
		if (c != '%'){
			*(buf_ptr++) = c;
			continue;
		}

		switch (c = *(fmt_ptr++)){
			case '%':
				*(buf_ptr++) = c;
				break;

			case 'u':
				diff = utoa(va_arg(args, uint32_t), buf_ptr, 10, digits);
				buf_ptr += diff;
				break;
			case 'p':
			case 'h':
				addr_t addr = va_arg(args, addr_t);
				__strncat_len(buf_ptr, "0x", 2, NULL, (size_t*)&diff);
				buf_ptr += diff;
				if (c == 'p'){
					int padding = (sizeof(void*)*2) - __psize(addr);
					memset (buf_ptr, '0', padding);
					buf_ptr += padding;
				}
				diff = utoa(addr, buf_ptr, 16, digits);
				if (c == 'h' && diff == 0)
					*(buf_ptr++) = '0';
				else
					buf_ptr += diff;
				break;
			case 's':
				__strncat_len(buf_ptr, va_arg(args, const char*), INT_MAX, NULL, (size_t*)&diff);
				buf_ptr += diff;
				break;
			default:
				break;
		}
	}
	return buf_ptr - buf;
}