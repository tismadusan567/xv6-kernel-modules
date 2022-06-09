typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef unsigned long long uint64;

typedef uint64 segdesc;
typedef uint64 gatedesc;
typedef uint pde_t;
typedef uint pte_t;

struct hook_arg {
	uint offset;
	void *arg1;
	void *arg2;
	void *arg3;
};

struct module {
	char name[16];
	int hook_id;
	void (*f)(struct hook_arg);
};

struct hook_func {
	char name[16];
	void (*f)(struct hook_arg);
	int pid;
	void *org_f;
};

enum hook{
	CONSOLE_HOOK_CRT,
	CONSOLE_HOOK_BUF,
	FORK,
	FILE_WRITE,
	FILE_READ,
	// SCHED,
	NUM_OF_HOOKS,
};

#define INPUT_BUF 128
struct input_buf{
	char buf[INPUT_BUF];
	uint r;  // Read index
	uint w;  // Write index
	uint e;  // Edit index
};

struct inode_arg {
	uint inum;          // Inode number
	short type;         // copy of disk inode
};
