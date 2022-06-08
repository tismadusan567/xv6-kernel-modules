#include "kernel/types.h"
#include "user.h"

void f1(struct hook_arg arg) {
	int *x = (int*)arg.arg;
	(*x)++;
}

void f2(struct hook_arg arg) {
	ushort *crt = (ushort*)arg.arg;
	crt[0]++;
}

int
main(int argc, char *argv[])
{
	struct module m1 = {"prvi", FORK, f1};
	struct module m2 = {"drugi", CONSOLE_HOOK, f2};
	struct module arr[] = {m1};

	if(init_module(arr, 1) != 0) {
		exit();
	}
}
