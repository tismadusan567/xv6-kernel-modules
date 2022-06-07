#include "kernel/types.h"
#include "user.h"

struct module;

void f1(void *arg) {
	int *x = (int*)arg;
	(*x)++;
}

void f2(void *arg) {
	ushort *crt = (ushort*)arg;
	crt[0]++;
}

int
main(int argc, char *argv[])
{
	struct module m1 = {"prvi", 0, f1};
	struct module m2 = {"drugi", 2, f2};
	struct module arr[1] = {m2};
	int a = 5;
	f1(&a);
	init_module(arr, 1);
	// exit();
}
