#include "kernel/types.h"
#include "user.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"

char key = 0;
char encrypted[1000];
char for_encryption[1000];

//exec_hook(FILE_WRITE, &iarg, &addr, &n);
void encrypt(struct hook_arg args) {
    char *p_key = (char*)(&key) + args.offset;
    char *p_encrypted = (char*)encrypted + args.offset;
    char *p_for_encryption = (char*)for_encryption + args.offset;

    struct inode_arg *iarg = (struct inode_arg *)args.arg1;
    char **addr = (char**)args.arg2;
    int *n = (int*)args.arg3;

    if(iarg->type != T_FILE || p_encrypted[iarg->inum] == 1 || p_for_encryption[iarg->inum] == 0) return;
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

// exec_hook(CREATE_FILE, &(ip->inum), 0, 0);
void new_file(struct hook_arg args) {
    char *p_for_encryption = (char*)for_encryption + args.offset;

    uint *p_inum = (uint*)args.arg1;

    p_for_encryption[*p_inum] = 1;
}

void load_key() 
{
    int fd;
    if((fd = open("/etc/enckey", O_RDONLY)) < 0){
        printf("fileenc: cannot open /etc/enckey\n");
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
int
main(int argc, char *argv[])
{
    load_key();
	struct module m1 = {"enc", FILE_WRITE, encrypt};
	struct module m2 = {"dec", FILE_READ, decrypt};
    struct module m3 = {"encnew", CREATE_FILE, new_file};
	struct module arr[] = {m1, m2, m3};
	if(init_module(arr, 3) != 0) {
		exit();
	}
}
