#ifndef __H_USAGE_ELF_LINKER_SRC_CMN_H
#define __H_USAGE_ELF_LINKER_SRC_CMN_H

#include <stddef.h>
/* For some fucked up reason this is not a part of POSIX,
 * but still can be seen on some machines.
 * Why Stallman... Just why...
*/
#ifndef EOK
#define EOK 0
#endif

#define align(x, to) ((x + (to - 1)) & ~(to - 1))
/* Align to lower */
#define align_low(x, to) ((x) & ~(to - 1))
#define padding(x,to) (align(x,to)-x)

/* https://stackoverflow.com/questions/807244/c-compiler-asserts-how-to-implement */
#define GLUE(a,b) __GLUE(a,b)
#define __GLUE(a,b) a ## b
#define CVERIFY(expr, msg) typedef char GLUE (compiler_verify_, msg) [(expr) ? (+1) : (-1)]
#define __ASSERT(exp) CVERIFY (exp, __LINE__)

typedef unsigned long int addr_t;
__ASSERT(sizeof(addr_t) == sizeof(void*));

#ifndef offsetof
	#define offsetof(type,name)\
		( (size_t)&(((type*)0)->name) )
#endif

#ifndef container_of
	#define container_of(ptr, type, name) \
		( (type*)((char*)ptr - offsetof(type,name)) )
#endif

static inline void* __ptradd (void* p1, void* p2){
	return (void*)((addr_t)p1 + (addr_t)p2);
}

#define ptradd(p1,p2) __ptradd ((void*)p1, (void*)p2)

typedef char sym;
#define __symval(x, type) (type)(&x)

/* bit(0) = 0b00000001 */
/* bit(4) = 0b00010000 */
/* bit(5) = 0b00100000 */
/* bit(7) = 0b10000000 */

#define bit(x) (1<<x)

/* bitmask(0) = 0b00000000 */
/* bitmask(4) = 0b00001111 */
/* bitmask(5) = 0b00011111 */
/* bitmask(7) = 0b01111111 */

#define bitmask(x) (bit(x) - 1)

#define bitoff(x, by) (x << by)

/* in "val", from "where" bit, take "by" */

/* val = 1100 0110 
 * bitcut(val, 0, 4) = 0000 0110
 * bitcut(val, 4, 4) = 0000 1100
 * */

#define bitcut(val, where, by) ((val >> where) & bitmask(by))

#define kib(x) (x * (1<<10))
#define mib(x) (x * (1<<20))
#define gib(x) (x * (1<<30))

#define MAX(x,y) (x>y?x:y)
#define MIN(x,y) (x>y?y:x)

static inline const char* getenv (const char** envp, const char* ename){
	for (	const char** iter = envp;
		*iter != NULL;
		iter++)
	{
		const char* vstr = *iter;
		for (int i = 0; vstr[i] && (vstr[i] == '=' || (ename[i] && vstr[i] == ename[i])); i++)
		{
			if (vstr[i] == '='){
				return vstr+i+1;
			}
		}
	}
	return NULL;
}

#endif /* __H_USAGE_ELF_LINKER_SRC_CMN_H */