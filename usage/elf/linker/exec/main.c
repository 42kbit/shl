#include <stdio.h>

extern int test_val;
int get_new_test_val (void);

void _start(void){
	int wtf = test_val;
	int wtf2 = get_new_test_val ();
	while (1);
}

/*
int main(void){
	printf ("Hello, world!\n");
        return 0;
}
*/