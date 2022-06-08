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
	void *arg;
};

struct module {
	char name[16];
	int hook_id;
	void (*f)(struct hook_arg arg);
};



struct hook_func {
	char name[16];
	void (*f)(struct hook_arg);
	int pid;
	void *org_func;
};



enum hook{
	CONSOLE_HOOK,
	FORK,
	// FILE_READ,
	// FILE_WRITE,
	// SCHED,
};
