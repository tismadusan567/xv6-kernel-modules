typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef unsigned long long uint64;

typedef uint64 segdesc;
typedef uint64 gatedesc;
typedef uint pde_t;
typedef uint pte_t;

struct module {
	char name[16];
	int hook_id;
	void (*f)(void* arg);
};

struct hook_func {
	char name[16];
	void (*f)(void*);
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
