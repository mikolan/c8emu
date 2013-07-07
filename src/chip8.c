#include "chip8.h"
#include "video.h"
#include "audio.h"
#include "input.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

unsigned char _fontset[80] =
{
    0xf0,0x90,0x90,0x90,0xf0, // 0
    0x20,0x60,0x20,0x20,0x70, // 1
    0xf0,0x10,0xf0,0x80,0xf0, // 2
    0xf0,0x10,0xf0,0x10,0xf0, // 3
    0x90,0x90,0xf0,0x10,0x10, // 4
    0xf0,0x80,0xf0,0x10,0xf0, // 5
    0xf0,0x80,0xf0,0x90,0xf0, // 6
    0xf0,0x10,0x20,0x40,0x40, // 7
    0xf0,0x90,0xf0,0x90,0xf0, // 8
    0xf0,0x90,0xf0,0x10,0xf0, // 9
    0xf0,0x90,0xf0,0x90,0x90, // A
    0xe0,0x90,0xe0,0x90,0xe0, // B
    0xf0,0x80,0x80,0x80,0xf0, // C
    0xe0,0x90,0x90,0x90,0xe0, // D
    0xf0,0x80,0xf0,0x80,0xf0, // E
    0xf0,0x80,0xf0,0x80,0x80  // F
};

void (*ch8_table[16])() =
{
    _exec_noparam,     // 0x0XXX
    _ch8_jmp,          // 0x1XXX
    _ch8_call,         // 0x2XXX
    _ch8_skip_if_eq,   // 0x3XXX
    _ch8_skip_if_neq,  // 0x4XXX
    _ch8_skip_if_xeqy, // 0x5XXX
    _ch8_set_reg,      // 0x6XXX
    _ch8_add_reg,      // 0x7XXX
    _exec_math,        // 0x8XXX
    _ch8_sne,          // 0x9XXX
    _ch8_loadi,        // 0xAXXX
    _ch8_jmpv0,        // 0xBXXX
    _ch8_rand,         // 0xCXXX
    _ch8_drw,          // 0xDXXX
    _exec_input,       // 0xEXXX
    _exec_mem          // 0XFXXX
};

void (*ch8_math_table[9])() =
{
    _ch8_load,       // 0x8xy0
    _ch8_or,         // 0x8xy1
    _ch8_and,        // 0x8xy2
    _ch8_xor,        // 0x8xy3
    _ch8_add,        // 0x8xy4
    _ch8_sub,        // 0x8xy5
    _ch8_shr,        // 0x8xy6
    _ch8_subn,       // 0x8xy7
    _ch8_shl         // 0x8xye
};

void ch8_init()
{
    printf("Initializing chip-8 emulator\n");
    // Reset program counter, opcode, index register and stack pointer.
    _pc = 0x200;
    _opcode = 0;
    _ir = 0;
    _sp = 0;
    if(DEBUG)
        printf("_pc: %d _opcode: %d, _ir: %d, _sp: %d\n", _pc, _opcode, _ir, _sp);

    // Reset framebuffer, stack, all registries and blank out memory.
    memset(_fb,0,sizeof(char)*2048);
    memset(_stack,0,sizeof(short)*16);
    memset(_v,0,sizeof(char)*16);
    memset(_mem,0,sizeof(char)*4096);
    if(DEBUG)
        printf("Memory, buffers and all registers set to 0\n");

    // Load fontset into memory
    memcpy(_mem,_fontset,sizeof(char)*80);

    // Reset all timers
    _dt = 0;
    _st = 0;
    
    // CPU initialized, initialize subsystems
    ch8_init_video();
    
    // Seed PRNG
    srand(time(0));
}

void ch8_run_cycle()
{
    _fetch_opcode(); // opcode is now in _opcode
    _exec_opcode(); // Runs the appropriate function from the function table
}

void ch8_draw()
{

}

void ch8_process_input()
{

}

void ch8_load(char* rom)
{
    FILE* f = fopen(rom,"r"); // Open file in read mode
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
    fread((_mem+512),1,(size_t) f_len, f); // Read it into c8 memory
    
    fclose(f);
}

void _fetch_opcode()
{
    if(DEBUG)
        printf("Fetching opcode at address: %p\n", _mem);
    _opcode = _mem[_pc] << 8 | _mem[_pc+1];
    if(DEBUG)
        printf("opcode: %x fetched\n", (unsigned int)_opcode);
}

void _exec_opcode()
{
    ch8_table[(_opcode & 0xf000) >> 12]();
}

void _exec_math()
{
    if((_opcode & 0x000f) > 7)
    {
        ch8_math_table[8]();
    }
    else
    {
        ch8_math_table[_opcode & 0x000f]();
    }
}

void _exec_noparam()
{
    if(_opcode & 0x000f)
    {
        _ch8_ret();
    }
    else
    {
        _ch8_cls();
    }
}

void _exec_input()
{
    if((_opcode & 0x00ff) == 0x9e)
    {
        _ch8_skip_if_key();
    }
    else if((_opcode & 0x00ff) == 0xa1)
    {
        _ch8_skip_if_nkey();
    }
}

void _exec_mem()
{
    switch(_opcode & 0x00ff)
    {
        case 0x07:
            _ch8_load_dt();
            break;
        case 0x0a:
            _ch8_wait_key();
            break;
        case 0x15:
            _ch8_set_dt();
            break;
        case 0x18:
            _ch8_set_st();
            break;
        case 0x1e:
            _ch8_addi();
            break;
        case 0x29:
            _ch8_load_fnt();
            break;
        case 0x33:
            _ch8_load_bcd();
            break;
        case 0x55:
            _ch8_w_mem();
            break;
        case 0x65:
            _ch8_l_mem();
            break;
        default:
            break;
    }
}

void _ch8_cls()
{
    memset(_fb,0,sizeof(char)*2048);
    _pc += 2;
    _draw_next_cycle = 1;
}

void _ch8_ret()
{
    _sp--;
    _pc = _stack[_sp];
    _pc += 2;
}

void _ch8_jmp()
{
    _pc = _opcode & 0x0fff;
}

void _ch8_call()
{
    _stack[_sp] = _pc;
    _sp++;
    _pc = _opcode & 0x0fff;
}

void _ch8_skip_if_eq()
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


void _ch8_skip_if_neq()
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


void _ch8_skip_if_xeqy()
{
    if(_v[(_opcode & 0x0f00) >> 8] == _v[(_opcode & 0x00f0 >> 4)])
    {
        _pc += 4;
    }
    else
    {
        _pc += 2;
    }
}


void _ch8_set_reg()
{
    _v[(_opcode & 0x0f00) >> 8] = _opcode & 0x00ff;
    _pc += 2;
}


void _ch8_add_reg()
{
    _v[(_opcode & 0x0f00) >> 8] = _v[(_opcode & 0x0f00) >> 8] + (_opcode & 0x00ff);
    _pc += 2;
}


void _ch8_load()
{
    _v[(_opcode & 0x0f00) >> 8] = _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _ch8_or()
{
    _v[(_opcode & 0x0f00) >> 8] |= _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _ch8_and()
{
    _v[(_opcode & 0x0f00) >> 8] &= _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _ch8_xor()
{
    _v[(_opcode & 0x0f00) >> 8] ^= _v[(_opcode & 0x00f0) >> 4];
    _pc += 2;
}


void _ch8_add()
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


void _ch8_sub()
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


void _ch8_shr()
{
    _v[0xf] = _v[(_opcode & 0x0f00) >> 8] & 0x1;
    _v[(_opcode & 0x0f00) >> 8] >>= 1;
    _pc += 2;
}


void _ch8_subn()
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


void _ch8_shl()
{
    _v[0xf] = _v[(_opcode & 0x0f00) >> 8] >> 7;
    _v[(_opcode & 0x0f00) >> 8] <<= 1;
    _pc += 2;
}


void _ch8_sne()
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


void _ch8_loadi()
{
    _ir = _opcode & 0x0fff;
    _pc += 2;
}


void _ch8_jmpv0()
{
    _pc = (_opcode & 0x0fff) + _v[0];
}


void _ch8_rand()
{
    _v[(_opcode & 0x0f00) >> 8] = (rand() % 255) & (_opcode & 0x0ff);
    _pc += 2;
}


void _ch8_drw()
{
    // TODO
}


void _ch8_skip_if_key()
{
    // TODO
}


void _ch8_skip_if_nkey()
{
    // TODO
}


void _ch8_load_dt()
{
    _v[(_opcode & 0x0f00) >> 8] = _dt;
    _pc += 2;
}


void _ch8_wait_key()
{
}


void _ch8_set_dt()
{
    _dt = _v[(_opcode & 0x0f00) >> 8];
    _pc += 2;
}


void _ch8_set_st()
{
    _st = _v[(_opcode & 0x0f00) >> 8];
    _pc += 2;
}


void _ch8_addi()
{
    _ir += _v[(_opcode & 0x0f00) >> 8];
    _pc += 2;
}


void _ch8_load_fnt()
{
    _ir = _v[(_opcode & 0x0f00) >> 8] * 5;
    _pc += 2;
}


void _ch8_load_bcd()
{
    _mem[_ir] = _v[(_opcode & 0x0f00) >> 8] / 100; 
    _mem[_ir + 1] = (_v[(_opcode & 0x0f00) >> 8] / 10) % 10;
    _mem[_ir + 2] = (_v[(_opcode & 0x0f00) >> 8] / 100) % 10;

    _pc += 2;
}


void _ch8_w_mem()
{
    memcpy(&_mem[_ir],_v,sizeof(char)*((_opcode & 0x0f00) >> 8));
    _pc += 2;
}


void _ch8_l_mem()
{
    memcpy(_v,&_mem[_ir],sizeof(char)*( (_opcode & 0x0f00) >> 8));
}

