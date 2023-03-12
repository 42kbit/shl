#include <stdio.h>

extern int test_val;
int get_new_test_val (void);

void _start(void){
	while (1);
	volatile int* wtf = &test_val;
	*(wtf) = get_new_test_val ();
	while (1);
}

/*
int main(void){
	printf ("Hello, world!\n");
        return 0;
}
*/