#include "chip8.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Usage: c8emu <rom>");
        return 0;
    }
    
    ch8_init();

    char* rom = argv[1];
    ch8_load(rom);

    for(;;)
    {
        ch8_run_cycle();
        ch8_draw();
        ch8_process_input();
    }

}
