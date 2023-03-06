#define _MSTR(x) #x
#define MSTR(x) _MSTR(x)

/* Set custom .interp.
 * __INTERP_PATH is defined by static linker (check Rules.mk)
 */

const char new_interp[] __attribute__((section(".interp")))
        = MSTR(__INTERP_PATH);

int main(void){
        return 0;
}