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
		cprintf("Init_module failed: argument error\n");
		return 1;
	}
	for(int i=0;i<n;i++) {
		//todo: dont panic
		cprintf("%s %d %p\n", modules[i].name, modules[i].hook_id, modules[i].f);
		if(module_name_exists(modules[i].name) != 0) {
			cprintf("Init_module failed: Module name already exists\n");
			return 2;
		}
		if(assign_to_hook(modules[i].name, modules[i].hook_id, modules[i].f, myproc()->pid) != 0) {
			cprintf("Init_module failed: Max modules at hook %d\n", modules[i].hook_id);
			return 3;
		}
	}
	set_resident();
	remap_all_to_residents();
	myyield();
	return 0;
}

int 
sys_del_module(void) {
	char *modname;
	if(argstr(0, &modname) < 0) {
		return 1;
	}
	int pid = del_hook_function(modname);
	if(pid == 0) {
		return 2;
	}
	if(module_count(pid) == 0) {
		release_resident(pid);
	}
	return 0;
}