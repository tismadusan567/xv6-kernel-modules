#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    if(argc != 3){
        printf("Usage: writer [filename] [data]\n");
        exit();
    }

    int fd;
    if ((fd = open(argv[1], O_CREATE|O_WRONLY)) < 0) {
        exit();
    }

    write(fd, argv[2], strlen(argv[2]));
    close(fd);
    exit();
}