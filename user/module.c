#include "kernel/types.h"
#include "user.h"

struct module;

void f1(void *arg) {

}

void f2(void *arg) {

}

int
main(int argc, char *argv[])
{
	struct module m1 = {"prvi", 1, f1};
	struct module m2 = {"drugi", 2, f2};
	struct module arr[2] = {m1, m2};
	init_module(arr, 2);
	exit();
}
