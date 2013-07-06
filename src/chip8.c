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

void _c8_cls()
{
    memset(_fb,0,sizeof(char)*2048);
    _pc += 2;
    _draw_next_cycle = 1;
}

void _c8_ret()
{
    _sp--;
    _pc = _stack[_sp];
    _pc += 2;
}

void _c8_jmp()
{
    _pc = _opcode & 0x0fff;
}

void _c8_call()
{
    _stack[_sp] = _pc;
    _sp++;
    _pc = _opcode & 0x0fff;
}

void _c8_skip_if_eq()
{
    if(_v[(_opcode & 0x0f00) >> 8] == (_opcode & 0x0ff))
    {
        _pc += 4;
    }
    else
    {
        _pc += 2;
    }
}


void _c8_skip_if_neq()
{
    if(_v[(_opcode & 0x0f00) >> 8] != (_opcode & 0x0ff))
    {
        _pc += 4;
    }
    else
    {
        _pc += 2;
    }
}


void _c8_skip_if_xeqy()
{
    if(_v[(_opcode & 0x0f00) >> 8] _v[_opcode & 0x00f0 >> 4])
    {
        _pc += 4;
    }
    else
    {
        _pc += 2;
    }
}


void _c8_set_reg()
{
    _v[(_opcode & 0x0f00) >> 8] = _opcode & 0x00ff;
    _pc += 2;
}


void _c8_add_reg()
{
    _v[(_opcode & 0x0f00) >> 8] = _v[(_opcode & 0x0f00) >> 8] + (_opcode & 0x00ff);
    _pc += 2;
}


void _c8_load()
{
    _v[(_opcode & 0x0f00) >> 8] = _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _c8_or()
{
    _v[(_opcode & 0x0f00) >> 8] |= _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _c8_and()
{
    _v[(_opcode & 0x0f00) >> 8] &= _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _c8_xor()
{
    _v[(_opcode & 0x0f00) >> 8] ^= _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _c8_add()
{
    if((_v[(_opcode & 0x0f00) >> 8] + _v[(_opcode & 0x00f0) >> 4]) > 0xff)
    {
        _v[0xf] = 1;
    }
    else
    {
        _v[0xf] = 0;
    }
    _v[(_opcode & 0x0f00) >> 8] += _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _c8_sub()
{
    if(_v[(_opcode & 0x0f00) >> 8] > _v[(_opcode & 0x00f0) >> 4])
    {
        _v[0xf] = 1;
    }
    else
    {
        _v[0xf] = 0;
    }
    _v[(_opcode & 0x0f00) >> 8] -= _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _c8_shr()
{
    _v[0xf] = _v[(_opcode & 0x0f00) >> 8] & 0x1;
    _v[(_opcode & 0x0f00) >> 8] >>= 1;
    _pc += 2;
}


void _c8_subn()
{
    if(_v[(_opcode & 0x00f0) >> 4] > _v[(_opcode & 0x0f00) >> 8])
    {
        _v[0xf] = 1;
    }
    else
    {
        _v[0xf] = 0;
    }
    _v[(_opcode & 0x00f0) >> 4] -= _v[(_opcode & 0x0f00) >> 8];
    _pc += 2;
}


void _c8_shl()
{
    _v[0xf] = _v[(_opcode & 0x0f00) >> 8] >> 7;
    _v[(_opcode & 0x0f00) >> 8] <<= 1;
    _pc += 2;
}


void _c8_sne()
{
    if(_v[(_opcode & 0x00f0) >> 4] == _v[(_opcode & 0x0f00) >> 8])
    {
        _pc += 4;
    }
    else
    {
        _pc += 2;
    }
}


void _c8_loadi()
{
    _ir = _opcode & 0x0fff;
    _pc += 2;
}


void _c8_jmpv0()
{
    _pc = (_opcode & 0x0fff) + _v[0];
}


void _c8_rand()
{
    _v[(_opcode & 0x0f00) >> 8] = (rand % 255) & (_opcode & 0x0ff);
    _pc += 2;
}


void _c8_drw()
{
    // TODO
}


void _c8_skip_if_key()
{
    // TODO
}


void _c8_skip_if_nkey()
{
    // TODO
}


void _c8_load_dt()
{
    _v[(_opcode & 0x0f00) >> 8] = _dt;
    _pc += 2;
}


void _c8_wait_key()
{
}


void _c8_set_dt()
{
    _dt = _v[(_opcode & 0x0f00) >> 8];
    _pc += 2;
}


void _c8_set_st()
{
    _st = _v[(_opcode & 0x0f00) >> 8];
    _pc += 2;
}


void _c8_addi()
{
    _ir += _v[(_opcode & 0x0f00) >> 8];
    _pc += 2;
}


void _c8_load_fnt()
{
    _ir = _v[(_opcode & 0x0f00) >> 8] * 5;
    _pc += 2;
}


void _c8_load_bcd()
{
    _ir = _v[(_opcode & 0x0f00) >> 8] / 100; 
    _ir + 1 = (_v[(_opcode & 0x0f00) >> 8] / 10) % 10;
    _ir + 2 = (_v[(_opcode & 0x0f00) >> 8] / 100) % 10;

    _pc += 2;
}


void _c8_w_mem()
{
    memcpy(_ir, _v, (_opcode & 0x0f00) >> 8);
    _pc += 2;
}


void _c8_l_mem()
{
    memcpy(_v, _ir, (_opcode & 0x0f00) >> 8);
}

