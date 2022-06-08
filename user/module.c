#include "kernel/types.h"
#include "user.h"

void f1(void *arg) {
	int *x = (int*)arg;
	(*x)++;
}

void f2(void *arg) {
	ushort *crt = (ushort*)arg;
	crt[0]++;
}

void f3(void *arg) {
	int *x = (int*)arg;
	(*x)--;
}

void f4(void *arg) {
	ushort *crt = (ushort*)arg;
	crt[0]--;
}

int
main(int argc, char *argv[])
{
	struct module m1 = {"prvi", FORK, f1};
	struct module m2 = {"drugi", CONSOLE_HOOK, f2};
	struct module arr[] = {m1};

	struct module m3 = {"treci", FORK, f3};
	struct module m4 = {"cetvrti", CONSOLE_HOOK, f4};
	struct module arr2[] = {m3, m4};
	init_module(arr, 1);
	// exit();
}
