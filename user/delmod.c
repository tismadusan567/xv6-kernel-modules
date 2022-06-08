#include "kernel/types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    if(argc != 2) {
        printf("Usage: delmod [modname]\n");
        exit();
    }
    char *module = argv[1];
	del_module(module);
	exit();
}
