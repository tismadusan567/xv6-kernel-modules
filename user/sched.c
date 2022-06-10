#include "kernel/types.h"
#include "user.h"

#define NPROC 64
#define RUNNABLE 3

int priorities[NPROC];
int cur_pos = 0;

// exec_hook(SCHED, states, &hook_idx, 0);
void sched(struct hook_arg args) {
    int *p_priorities = (int*)((char*)priorities + args.offset);
    int *p_pos = (int*)((char*)&cur_pos + args.offset);

    int *states = (int*)args.arg1;
    int *p_hook_idx = (int*)args.arg2;
    int max = -1;

   

    for(int i=0;i<NPROC;i++) {
        if(states[i] == RUNNABLE && p_priorities[i] > max) max = p_priorities[i];
    }
    
    for(int i = (*p_pos) + 1;i < NPROC;i++) {
        if(states[i] == RUNNABLE && p_priorities[i] == max) {
            *p_hook_idx = i;
            *p_pos = i;
            return;
        }
    }

    for(int i = 0; i < (*p_pos) + 1;i++) {
        if(states[i] == RUNNABLE && p_priorities[i] == max) {
            *p_hook_idx = i;
            *p_pos = i;
            return;
        }
    }


}

// exec_hook(SET_PRIO, &index, &prio, 0);
void set_priority(struct hook_arg args) {
    int *p_priorities = (int*)((char*)priorities + args.offset);

    int index = *((int*)args.arg1);
    int priority = *((int*)args.arg2);

    p_priorities[index] = priority;

}

int
main(int argc, char *argv[])
{
	struct module m1 = {"sched", SCHED, sched};
	struct module m2 = {"setprio", SET_PRIO, set_priority};
	struct module arr[] = {m1, m2};
	if(init_module(arr, 2) != 0) {
		exit();
	}
}
