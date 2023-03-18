#include <stddef.h>

const char* __envp_find (const char** envp, const char* ename){
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