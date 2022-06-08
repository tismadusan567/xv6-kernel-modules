#include "kernel/types.h"
#include "user.h"

// ushort color = 0x5F00;
static ushort color(int set, ushort new) {
    static ushort clr = 0x5F00;
    // if(set) clr = new;`
    return clr;
}

void paint_screen(void* arg) {
    ushort *crt = (ushort*)arg;
    crt[0]++;
	for(int i=0;i<25*80;i++) {
        crt[i] &= 0x00FF;
	    crt[i] |= color(0,0);
    }
}

int
main(int argc, char *argv[])
{
    // color = 0x6F00;
    ushort newclr = 0x6F00;
    color(1, newclr);
    
	struct module m1 = {"sc", CONSOLE_HOOK, paint_screen};
	struct module arr[1] = {m1};
	init_module(arr, 1);
}
