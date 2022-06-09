#include "kernel/types.h"
#include "user.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"

char key = 0;
char encrypted[1000];

//exec_hook(FILE_WRITE, &iarg, &addr, &n);
void encrypt(struct hook_arg args) {
    char *p_key = (char*)(&key) + args.offset;
    char *p_encrypted = (char*)encrypted + args.offset;

    struct inode_arg *iarg = (struct inode_arg *)args.arg1;
    char **addr = (char**)args.arg2;
    int *n = (int*)args.arg3;

    if(iarg->type != T_FILE || p_encrypted[iarg->inum] == 1) return;
    p_encrypted[iarg->inum] = 1;

    for(int i=0;i<*n;i++) {
        (*addr)[i] += *p_key;
    }
}

//exec_hook(FILE_READ, &iarg, &addr, &n);
void decrypt(struct hook_arg args) {
    char *p_key = (char*)(&key) + args.offset;
    char *p_encrypted = (char*)encrypted + args.offset;

    struct inode_arg *iarg = (struct inode_arg *)args.arg1;
    char **addr = (char**)args.arg2;
    int *n = (int*)args.arg3;

    if(iarg->type != T_FILE || p_encrypted[iarg->inum] == 0) return;

    for(int i=0;i<*n;i++) {
        (*addr)[i] -= *p_key;
    }
}

void load_key() 
{
    int fd;
    if((fd = open("b", O_RDONLY)) < 0){
        printf("fileenc: cannot open %s\n", "b");
        exit();
	}
    int n;
    char buf[64];

	if((n = read(fd, buf, sizeof(buf))) < 0) {
        printf("fileenc: read error\n");
		exit();
    }
    buf[n]=0;
    key = atoi(buf) % 26;
    close(fd);
}


//ne moze sa echo
//proveriti za postojece
int
main(int argc, char *argv[])
{
    load_key();
	struct module m1 = {"enc", FILE_WRITE, encrypt};
	struct module m2 = {"dec", FILE_READ, decrypt};
	struct module arr[] = {m1, m2};
	if(init_module(arr, 2) != 0) {
		exit();
	}
}
