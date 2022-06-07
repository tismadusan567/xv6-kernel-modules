#include "kernel/types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    char *module = "prvi";
	del_module(module);
	exit();
}
