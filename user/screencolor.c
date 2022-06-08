#include "kernel/types.h"
#include "user.h"
#include "kernel/fcntl.h"

ushort color = 0x5F00;

void paint_screen(struct hook_arg arg) {
    ushort *crt = (ushort*)arg.arg;
    ushort* p_clr = (ushort*)((char*)(&color) + arg.offset);
    crt[0]++;
	for(int i=0;i<25*80;i++) {
        crt[i] &= 0x00FF;
	    crt[i] |= *p_clr;
    }
}

void load_color() {
    int fd;
    if((fd = open("a", O_RDONLY)) < 0){
        printf("screencolor: cannot open %s\n", "a");
        exit();
	}
    int n;
    char buf[64];

	if((n = read(fd, buf, sizeof(buf))) < 0) {
        printf("screencolor: read error\n");
		exit();
    }
    buf[n]=0;
    color = atoi(buf) << 8;

}

int
main(int argc, char *argv[])
{
    load_color();
	struct module m1 = {"screencolor", CONSOLE_HOOK, paint_screen};
	struct module arr[1] = {m1};
	if(init_module(arr, 1) != 0) {
		exit();
	}
}
