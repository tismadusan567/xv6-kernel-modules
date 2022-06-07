#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
	return fork();
}

int
sys_exit(void)
{
	exit();
	return 0;  // not reached
}

int
sys_wait(void)
{
	return wait();
}

int
sys_kill(void)
{
	int pid;

	if(argint(0, &pid) < 0)
		return -1;
	return kill(pid);
}

int
sys_getpid(void)
{
	return myproc()->pid;
}

int
sys_sbrk(void)
{
	int addr;
	int n;

	if(argint(0, &n) < 0)
		return -1;
	addr = myproc()->sz;
	if(growproc(n) < 0)
		return -1;
	return addr;
}

int
sys_sleep(void)
{
	int n;
	uint ticks0;

	if(argint(0, &n) < 0)
		return -1;
	acquire(&tickslock);
	ticks0 = ticks;
	while(ticks - ticks0 < n){
		if(myproc()->killed){
			release(&tickslock);
			return -1;
		}
		sleep(&ticks, &tickslock);
	}
	release(&tickslock);
	return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
	uint xticks;

	acquire(&tickslock);
	xticks = ticks;
	release(&tickslock);
	return xticks;
}

int 
sys_init_module(void)
{
	int n;
	struct module* modules;
	if(argint(1, &n) < 0 || argint(0, &modules) < 0) {
		cprintf("sys_init_module argument error\n");
		return 1;
	}
	for(int i=0;i<n;i++) {
		// cprintf("%s %d %p\n", modules[i].name, modules[i].hook_id, modules[i].f);
		assign_to_hook(modules[i].hook_id, modules[i].f);
	}
	// cprintf("%x\n", PGROUNDUP(get_end()));
	// map_to_residents(myproc()->pgdir);
	set_resident();
	remap_all_to_residents();
	myyield();
	return 0;
}