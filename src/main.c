#include "chip8.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>

int done = 0;

void term(int signum)
{
    printf("SIGTERM Received, exiting\n");
    done = 1;
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Usage: c8emu <rom>\n");
        return 0;
    }
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGINT, &action, NULL);
    
    ch8_init();

    char* rom = argv[1];
    ch8_load(rom);
    
    while(!done)
    {
        ch8_run_cycle();
        ch8_draw();
        ch8_process_input();
    }
    ch8_close();
    return 0;
}

