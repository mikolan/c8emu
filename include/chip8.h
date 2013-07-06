#ifndef CHIP8_H_
#define CHIP8_H_

unsigned short _opcode; // Current OPCode
unsigned char _mem[4096]; // 4096 bytes of memory
unsigned char _v[16]; // 16 Registers V0-V15
unsigned short _ir; // Index Register
unsigned short _pc; // Program Counter
unsigned char _fb[2048]; // Framebuffer, 64x32 pixels.
unsigned char _dt; // Delay timer
unsigned char _st; // Sound timer
unsigned short _stack[16]; // Small stack
unsigned short _sp; // Stack pointer
unsigned char _keys[16]; // Keypad state
unsigned const char _fontset[80] =
{
    0xf0,0x90,0x90,0x90,0Xf0, // 0
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

// Emulator Functions
void ch8_init();
void ch8_run_cycle();
void ch8_load(char* rom);
void _fetch_opcode();
void _exec_opcode();
void (*ch8_table[35])();

// Chip-8 instructions
void _c8_cls(); // 0x00e0 Clear Screen
void _c8_ret(); // 0x00ee Return from subroutine
void _c8_jmp(); // 0x1nnn Jump to address nnn
void _c8_call(); // 0x2nnn Call subroutine at nnn
void _c8_skip_if_eq(); // 0x3xkk Skips the next instruction if Vx == kk
void _c8_skip_if_neq(); // 0x4xkk Skips the next instruction if Vx !== kk
void _c8_skip_if_xeqy(); // 0x5xy0 Skips the next instruction if Vx == Vy
void _c8_set_reg(); // 0x6xkk Sets the value of Vx to kk
void _c8_add_reg(); // 0x7xkk Adds the value kk to Vx and stores it in Vx
void _c8_load(); // 0x8xy0 Loads the value of register Vy and stores it in Vx
void _c8_or(); // 0x8xy1 Stores Vx | Vy into Vx
void _c8_and(); // 0x8xy2 Stores Vx & Vy into Vx
void _c8_xor(); // 0x8xy3 Stores Vx ^ Vy into Vx
void _c8_add(); // 0x8xy4 Stores Vx + Vy into Vx. Stores carry flag in Vf
void _c8_sub(); // 0x8xy5 Stores Vx - Vy into Vy. Stores borrow flag in Vf
void _c8_shr(); // 0x8xy6 Stores Vx >> 1 into Vx. Stores LSB of Vx in Vf before shift
void _c8_subn(); // 0x8xy7 Stores Vy - Vx into Vx. Stores borrow flag in Vf
void _c8_shl(); // 0x8xyE Stores Vx << 1 into Vx. Stores MSB of Vx in Vf before shift
void _c8_sne(); // 0x9xy0 Skips the next instruction if Vx == Vy
void _c8_loadi(); // 0xannn Sets the Index Register to nnn
void _c8_jmpv0(); // 0xbnnn Jumps to address nnn + V0
void _c8_rand(); // 0xcxkk Sets Vx to a random byte + kk
void _c8_drw(); // 0xdxyn Draws a n byte sprite at coordinates Vx,Vy. Sets Vf = 1 if collision
void _c8_skip_if_key(); // 0xex9e Skips the next instruction if key with the value Vx is pressed
void _c8_skip_if_nkey(); // 0xex9e Skips the next instruction if key with the value Vx is not pressed
void _c8_load_dt(); // 0xfx07 Stores the current value of the delay timer in Vx
void _c8_wait_key(); // 0xfx0a Waits for a key press then stores the key value in Vx
void _c8_set_dt(); // 0xfx15 Sets the delay timer to Vx
void _c8_set_st(); // 0xfx18 Sets the sound timer to Vx
void _c8_addi(); // 0xfx1e Sets the index register to ir + Vx
void _c8_load_fnt(); // 0xfx29 Sets the index register to the location of the character Vx in the fontset
void _c8_load_bcd(); // 0xfx33 Stores the BCD representation of Vx into _ir, _ir + 1 and _ir + 2.
void _c8_w_mem(); // 0xfx55 Stores registers V0 through Vx into memory starting at the index register
void _c8_l_mem(); // 0xfx65 Loads x bytes starting at the index register into registers V0 through Vx



#endif
