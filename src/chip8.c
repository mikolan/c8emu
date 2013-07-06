#include "chip8.h"
#include "video.h"
#include "audio.h"
#include "input.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

void ch8_init()
{
    // Reset program counter, opcode, index register and stack pointer.
    _pc = 0x200;
    _opcode = 0;
    _ir = 0;
    _sp = 0;

    // Reset framebuffer, stack, all registries and blank out memory.
    memset(_fb,0,sizeof(char)*2048);
    memset(_stack,0,sizeof(short)*16);
    memset(_reg,0,sizeof(char)*16);
    memset(_mem,0,sizeof(char)*4096);

    // Load fontset into memory
    memcpy(_mem,_fontset,sizeof(char)*80);

    // Reset all timers
    _dt = 0;
    _st = 0;
    
    // CPU initialized, initialize subsystems
    ch8_video_init();
    
    // Seed PRNG
    srand(time(0));
}

void ch8_run_cycle()
{

}

void ch8_load(char* rom)
{
    FILE* f = fopen(rom,"r") // Open file in read mode
    if (f == NULL)
    {
        fprintf(stderr,"Unable to open file: %s", rom);
        exit(EXIT_FAILURE);
    }
    
    fseek(f,0,SEEK_END);
    long f_len = ftell(f);
    if (f_len > 3584)
    {
        fprintf(stderr,"ROM is too large for c8 memory");
        exit(EXIT_FAILURE);
    }
    
    fseek(f,0,SEEK_SET); // Seek to beginning of file
    fread((mem+512),1,(size_t) f_len, f); // Read it into c8 memory
    
    fclose(f);
}

void _fetch_opcode()
{
    _opcode = _mem[_pc] << 8 | _mem[_pc+1];
}

void _exec_opcode()
{
    ch8_table[(_opcode & 0xf000) >> 12]();
}
