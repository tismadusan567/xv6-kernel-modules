#include "kernel/types.h"
#include "user.h"

#define KEY_UP 0xE2
#define KEY_DN 0xE3
#define BACKSPACE 0x100

#define COMMAND_HISTORY_SIZE 128

char command_history[COMMAND_HISTORY_SIZE * COMMAND_HISTORY_SIZE];
int last_command = 0;
int curr_command = 0;

void save_command(struct hook_arg args) 
{
    char *p_command_history = (char*)(command_history) + args.offset;
    int *p_last_command = (int*)((char*)&last_command + args.offset);
    int *p_curr_command = (int*)((char*)&curr_command + args.offset);
    
    struct input_buf *input = (struct input_buf*)args.arg1;
    int c = *((int*)args.arg2);
    void (*p_consputc)(int) = (void (*)(int))args.arg3;

    if(c == KEY_DN || c == KEY_UP || c == 0 || *p_curr_command != *p_last_command) return;

    //find start of current command
    uint start_of_curr;
    for(start_of_curr=input->e-2;start_of_curr >= 0;start_of_curr--) {
        if(input->buf[start_of_curr % INPUT_BUF] == '\n' || input->buf[start_of_curr % INPUT_BUF] == '\r') break;
    }

    // if(start_of_curr < 0) last_command = 5/0;
    
    //empty command
    if(c == '\n' && start_of_curr == input->e-2) {
        p_command_history[*p_last_command * COMMAND_HISTORY_SIZE] = 0;
        *p_curr_command = *p_last_command;
        return;
    }

    //write current command to last_command pos in history, it can be overwritten
    int j = 0;
    for(int i=start_of_curr+1;i < input->e;i++){
        p_command_history[(*p_last_command) * COMMAND_HISTORY_SIZE + j++] = input->buf[i % INPUT_BUF];
    }
    p_command_history[*p_last_command * COMMAND_HISTORY_SIZE + j] = 0;

    //save permamently - no longer can be overwritten
    if(c == '\n') {
        p_command_history[*p_last_command * COMMAND_HISTORY_SIZE + j - 1] = 0;
        *p_last_command = (*p_last_command + 1) % COMMAND_HISTORY_SIZE;
        p_command_history[*p_last_command * COMMAND_HISTORY_SIZE] = 0;  
        *p_curr_command = *p_last_command;
    }


}

void change_command(struct hook_arg args)
{
    char *p_command_history = (char*)(command_history) + args.offset;
    int *p_last_command = (int*)((char*)&last_command + args.offset);
    int *p_curr_command = (int*)((char*)&curr_command + args.offset);
    
    struct input_buf *input = (struct input_buf*)args.arg1;
    int c = *((int*)args.arg2);
    void (*p_consputc)(int) = (void (*)(int))args.arg3;

    if(c == 0) return;
    if(c == KEY_DN || c == KEY_UP) {
        //remove up and down arrows from screen and buffer
        p_consputc(BACKSPACE);
        input->e--;
    }

    

    int next_command = (*p_last_command);
    if(c == KEY_UP) {
        //going back
        next_command = (*p_curr_command) - 1;
        //wrap-around
        if(next_command < 0) next_command = COMMAND_HISTORY_SIZE - 1;
        //we reached the top of the list
        if(next_command == (*p_last_command) || p_command_history[next_command * COMMAND_HISTORY_SIZE] == 0) 
            next_command = (*p_curr_command);
        
    }
    else if(c == KEY_DN) {
        //going forward
        next_command = (*p_curr_command) + 1;
        //wrap-around
        if(next_command >= COMMAND_HISTORY_SIZE) next_command = 0;
        //we tried going below the most recent command
        if((*p_curr_command) == (*p_last_command)) next_command = (*p_curr_command);
    }

    //delete current command from screen and buffer(it's already saved in history)
    while(input->e != input->w && input->buf[(input->e-1) % INPUT_BUF] != '\n') {
        input->e--;
        p_consputc(BACKSPACE);
	}

    //print the next command
    for(int i=0;p_command_history[next_command * COMMAND_HISTORY_SIZE + i] != 0;i++) {
        // if(p_command_history[next_command * COMMAND_HISTORY_SIZE + i] == '\n') p_consputc('P');
        input->buf[input->e++ % INPUT_BUF] = p_command_history[next_command * COMMAND_HISTORY_SIZE + i];
        p_consputc(p_command_history[next_command * COMMAND_HISTORY_SIZE + i]);
    }

    //remember curr position
    *p_curr_command = next_command;
}


//ne smeju prevelike komande
int
main(int argc, char *argv[])
{
	struct module m1 = {"ch1", CONSOLE_HOOK_BUF, save_command};
	struct module m2 = {"ch2", CONSOLE_HOOK_BUF, change_command};

	struct module arr[2] = {m1, m2};
	if(init_module(arr, 2) != 0) {
		exit();
	}
}
