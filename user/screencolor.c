#include "kernel/types.h"
#include "user.h"
#include "kernel/fcntl.h"

ushort color = 0x5F00;

void paint_screen(struct hook_arg args) 
{
    ushort *crt = (ushort*)args.arg1;
    char c = *((char*)args.arg2);
    if(c != '\n'&& c != '\r') return;
    ushort* p_clr = (ushort*)((char*)(&color) + args.offset);
    crt[0]++;
	for(int i=0;i<25*80;i++) {
        crt[i] &= 0x00FF;
	    crt[i] |= *p_clr;
    }
}

void load_color() 
{
    int fd;
    if((fd = open("/etc/sc.conf", O_RDONLY)) < 0){
        printf("screencolor: cannot open /etc/sc.conf\n");
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
    close(fd);

}

int
main(int argc, char *argv[])
{
    load_color();
	struct module m1 = {"screencolor", CONSOLE_HOOK_CRT, paint_screen};
	struct module arr[1] = {m1};
	if(init_module(arr, 1) != 0) {
		exit();
	}
}
