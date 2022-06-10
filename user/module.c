#include "kernel/types.h"
#include "user.h"

void f1(struct hook_arg arg) 
{
	char *c = atoi("a");
	int *x = (int*)arg.arg1;
	(*x)++;
}

void f2(struct hook_arg arg) 
{
	ushort *crt = (ushort*)arg.arg1;
	crt[0]++;
}

int
main(int argc, char *argv[])
{
	// struct module m1 = {"prvi", FORK, f1};
	// struct module m2 = {"drugi", CONSOLE_HOOK_CRT, f2};
	// struct module arr[] = {m1};

	// if(init_module(arr, 1) != 0) {
	// 	exit();
	// }
}
