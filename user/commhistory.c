#include "kernel/types.h"
#include "user.h"

#define KEY_UP 0xE2
#define KEY_DN 0xE3
#define BACKSPACE 0x100

// commhistory
// Omogućava da strelice gore i dole daju pristup istoriji komandi na konzoli. Najniža komanda
// u istoriji je ona koja je trenutno ukucana, čak i ako nije završena.

#define COMMAND_HISTORY_SIZE 10

char command_history[COMMAND_HISTORY_SIZE * INPUT_BUF];
int last_command = 0;
int curr_command = 0;

void save_command(struct hook_arg arg) 
{
    int offset = arg.offset;
    char *p_command_history = (char*)(command_history) + arg.offset;
    int *p_last_command = (int*)((char*)&last_command + arg.offset);
    int *p_curr_command = (int*)((char*)&curr_command + arg.offset);
    
    struct input_buf *input = (struct input_buf*)arg.arg1;
    int c = *((int*)arg.arg2);
    void (*p_consputc)(int) = (void (*)(int))arg.arg3;

    // if(c != '\n' && c != '\r') return;
    if(c == KEY_DN || c == KEY_UP || c == 0) return;

    uint start_of_prev;
    for(start_of_prev=input->e-2;start_of_prev >= 0;start_of_prev--) {
        if(input->buf[start_of_prev % INPUT_BUF] == '\n' || input->buf[start_of_prev % INPUT_BUF] == '\r') break;
    }
    
    //empty command
    if(c == '\n' && start_of_prev == input->e-2) {
        p_command_history[*p_last_command * COMMAND_HISTORY_SIZE] = 0;
        *p_curr_command = *p_last_command;
        return;
    }

    int j = 0;
    for(int i=start_of_prev+1;i < input->e;i++){
        p_command_history[(*p_last_command) * COMMAND_HISTORY_SIZE + j++] = input->buf[i % INPUT_BUF];
        // p_consputc(input->buf[i % INPUT_BUF]);
    }
    p_command_history[*p_last_command * COMMAND_HISTORY_SIZE + j] = 0;
    //save permamently
    if(c == '\n') {
        p_command_history[*p_last_command * COMMAND_HISTORY_SIZE + j - 1] = 0;
        *p_last_command = (*p_last_command + 1) % COMMAND_HISTORY_SIZE;
        p_command_history[*p_last_command * COMMAND_HISTORY_SIZE] = 0;  
        *p_curr_command = *p_last_command;
    }


}

void change_command(struct hook_arg arg)
{
    int offset = arg.offset;
    char *p_command_history = (char*)(command_history) + arg.offset;
    int *p_last_command = (int*)((char*)&last_command + arg.offset);
    int *p_curr_command = (int*)((char*)&curr_command + arg.offset);
    
    struct input_buf *input = (struct input_buf*)arg.arg1;
    int c = *((int*)arg.arg2);
    void (*p_consputc)(int) = (void (*)(int))arg.arg3;

    if(c != KEY_DN && c != KEY_UP) return;
    p_consputc(BACKSPACE);
    input->e--;

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

    while(input->e != input->w && input->buf[(input->e-1) % INPUT_BUF] != '\n') {
        input->e--;
        p_consputc(BACKSPACE);
	}

    for(int i=0;p_command_history[next_command * COMMAND_HISTORY_SIZE + i] != 0;i++) {
        if(p_command_history[next_command * COMMAND_HISTORY_SIZE + i] == '\n') last_command = 1;
        input->buf[input->e++ % INPUT_BUF] = p_command_history[next_command * COMMAND_HISTORY_SIZE + i];
        p_consputc(p_command_history[next_command * COMMAND_HISTORY_SIZE + i]);
    }
    *p_curr_command = next_command;

    //mzd ce trebati
    // for(uint i=input->w;i < input->e;i++) {
        // p_consputc(input->buf[i % INPUT_BUF]);
    // }
}

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
