#ifndef __H_SHL_ELF_H
#define __H_SHL_ELF_H

/* 
 * Compile time sanity check
 * Usage:
 * __ASSERT(sizeof(int) == 4);
 * Will drop compile error if assert fails.
 */
#define GLUE(a,b) __GLUE(a,b)
#define __GLUE(a,b) a ## b
#define CVERIFY(expr, msg) typedef char GLUE (compiler_verify_, msg) [(expr) ? (+1) : (-1)]
#define __ASSERT(exp) CVERIFY (exp, __LINE__)

#ifndef NULL
#define NULL ((void*)0)
#endif

#define container_of(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

#define shl_get_entry(ptr, type, name) \
    container_of(ptr, type, name)



#endif /* __H_SHL_ELF_H */
