#include "kernel/types.h"
#include "user.h"

#define KEY_PGUP 0xE6
#define KEY_PGDN 0xE7
#define BACKSPACE 0x100

ushort crt_history[100 * 80];
int currpos = 75*80;

//exec_hook(CONSOLE_CGAPUTC, crt, &scroll, &c);
void update_history(struct hook_arg args) {
    ushort *p_crt_history = (char*)crt_history + args.offset;
    int *p_cur_pos = (int*)((char*)&currpos + args.offset);

    int c = *((int*)args.arg3);

    if(c == KEY_PGDN || c == KEY_PGUP) return;
    if(*p_cur_pos != 75*80) return;

    ushort *crt = (ushort*)args.arg1;
    int scroll = *((int*)args.arg2);

    if(scroll) {
        memmove(p_crt_history, p_crt_history + 80, sizeof(ushort)*75*80);
    }
    memmove(p_crt_history + 75*80, crt, sizeof(ushort)*25*80);
}

//exec_hook(CONSOLE_CGAPUTC, crt, &scroll, &c);
// exec_hook(CONSOLE_HOOK_CRT, crt, &c, 0);
void scroll(struct hook_arg args) {
    int *p_cur_pos = (int*)((char*)&currpos + args.offset);
    ushort *p_crt_history = (ushort*)((char*)crt_history + args.offset);

    ushort *crt = (ushort*)args.arg1;
    int c = *((int*)args.arg2);
    struct input_buf *input = (struct input_buf*)args.arg3;

    if(c == 0) return;

    int next_pos = 75*80;
    if(c == KEY_PGUP) {
        if(*p_cur_pos > 79) next_pos = *p_cur_pos - 80;
    }
    else if(c == KEY_PGDN) {
        if(*p_cur_pos < 75*80) next_pos = *p_cur_pos + 80;
    }
    else if(*p_cur_pos != 75*80){
        //ako smo pritisnuli dugme da bi se vatili na trenutni ekran
        input->e--;
    }
    memmove(crt, p_crt_history + next_pos, sizeof(ushort)*25*80);
    *p_cur_pos = next_pos;
}


int
main(int argc, char *argv[])
{
	struct module m1 = {"history", CONSOLE_CGAPUTC, update_history};
	struct module m2 = {"scroll", CONSOLE_HOOK_CRT, scroll};
	struct module arr[] = {m1, m2};
	if(init_module(arr, 2) != 0) {
		exit();
	}
}
