#include "kernel/types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	
    if(argc != 3) {
        printf("Usage : setprio [pid] [priority]");
        exit();
    }

    set_prio(atoi(argv[1]), atoi(argv[2]));

    exit();
}
