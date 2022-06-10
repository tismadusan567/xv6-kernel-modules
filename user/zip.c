#include "kernel/types.h"
#include "user.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"

uint compressed[1000];
char for_compression[1000];
char new_contents[2000];


//exec_hook(FILE_WRITE, &iarg, &addr, &n);
void zip(struct hook_arg args) {
    char *p_compressed = (char*)compressed + args.offset;
    char *p_for_compression = (char*)for_compression + args.offset;
    char *p_new_contents = (char*)new_contents + args.offset;

    struct inode_arg *iarg = (struct inode_arg *)args.arg1;
    char **addr = (char**)args.arg2;
    int *n = (int*)args.arg3;

    if(iarg->type != T_FILE || p_compressed[iarg->inum] != 0 || p_for_compression[iarg->inum] == 0) return;

    int lim = 'z' - '0';
    int j = 0;
    char count;
    for(int i=0;i<*n;i++) {
        count = 1;
        while(i < *n - 1 && (*addr)[i] == (*addr)[i + 1] && count < lim) {
            count++;
            i++;
        }
        p_new_contents[j++] = (*addr)[i];
        p_new_contents[j++] = count + '0';
    }

    for(int i=0;i<j;i++) {
        (*addr)[i] = p_new_contents[i];
    }
    p_compressed[iarg->inum] = *n;
    *n = j;
   
}

//exec_hook(FILE_READ, &iarg, &addr, &n);
void unzip(struct hook_arg args) {
    char *p_compressed = (char*)compressed + args.offset;

    struct inode_arg *iarg = (struct inode_arg *)args.arg1;
    char **addr = (char**)args.arg2;
    int *n = (int*)args.arg3;

    if(iarg->type != T_FILE || p_compressed[iarg->inum] == 0 || *n == 0) return;

    int j = 0;
    char c, freq;
    for(int i=0;i<*n;i+=2) {
        c = (*addr)[i];
        if(i+1 == *n) break;
        freq = (*addr)[i+1] - '0';
        while(freq--) {
            new_contents[j++] = c;
        }
    }

    for(int i=0;i<j;i++) {
        (*addr)[i] = new_contents[i];
    }
    *n = p_compressed[iarg->inum];
}

// exec_hook(CREATE_FILE, &(ip->inum), 0, 0);
void new_file_zip(struct hook_arg args) {
    char *p_for_compression = (char*)for_compression + args.offset;

    uint *p_inum = (uint*)args.arg1;

    p_for_compression[*p_inum] = 1;
}

//ne moze sa echo
int
main(int argc, char *argv[])
{
	struct module m1 = {"zip", FILE_WRITE, zip};
	struct module m2 = {"unzip", FILE_READ, unzip};
    struct module m3 = {"zipnew", CREATE_FILE, new_file_zip};
	struct module arr[] = {m1, m2, m3};
	if(init_module(arr, 3) != 0) {
		exit();
	}
}
