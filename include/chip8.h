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
unsigned char _fontset[80]; // Font data
int _draw_next_cycle; // Tells us whether to draw the buffer or not.


// Emulator Functions
void ch8_init();
void ch8_close();
void ch8_run_cycle();
void ch8_draw();
void ch8_dump();
void ch8_process_input();
void ch8_load(char* rom);
void _fetch_opcode();
void _exec_opcode();
void _exec_math();
void _exec_noparam();
void _exec_input();
void _exec_mem();
void (*ch8_table[16])();
void (*ch8_math_table[9])();

// chip-8 instructions
void _ch8_cls(); // 0x00e0 clear screen
void _ch8_ret(); // 0x00ee return from subroutine
void _ch8_jmp(); // 0x1nnn jump to address nnn
void _ch8_call(); // 0x2nnn call subroutine at nnn
void _ch8_skip_if_eq(); // 0x3xkk skips the next instruction if vx == kk
void _ch8_skip_if_neq(); // 0x4xkk skips the next instruction if vx !== kk
void _ch8_skip_if_xeqy(); // 0x5xy0 skips the next instruction if vx == vy
void _ch8_set_reg(); // 0x6xkk sets the value of vx to kk
void _ch8_add_reg(); // 0x7xkk adds the value kk to vx and stores it in vx
void _ch8_load(); // 0x8xy0 loads the value of register vy and stores it in vx
void _ch8_or(); // 0x8xy1 stores vx | vy into vx
void _ch8_and(); // 0x8xy2 stores vx & vy into vx
void _ch8_xor(); // 0x8xy3 stores vx ^ vy into vx
void _ch8_add(); // 0x8xy4 stores vx + vy into vx. stores carry flag in vf
void _ch8_sub(); // 0x8xy5 stores vx - vy into vy. stores borrow flag in vf
void _ch8_shr(); // 0x8xy6 stores vx >> 1 into vx. stores lsb of vx in vf before shift
void _ch8_subn(); // 0x8xy7 stores vy - vx into vx. stores borrow flag in vf
void _ch8_shl(); // 0x8xye stores vx << 1 into vx. stores msb of vx in vf before shift
void _ch8_sne(); // 0x9xy0 skips the next instruction if vx == vy
void _ch8_loadi(); // 0xannn sets the index register to nnn
void _ch8_jmpv0(); // 0xbnnn jumps to address nnn + v0
void _ch8_rand(); // 0xcxkk sets vx to a random byte + kk
void _ch8_drw(); // 0xdxyn draws a n byte sprite at coordinates vx,vy. sets vf = 1 if collision
void _ch8_skip_if_key(); // 0xex9e skips the next instruction if key with the value vx is pressed
void _ch8_skip_if_nkey(); // 0xexa1 skips the next instruction if key with the value vx is not pressed
void _ch8_load_dt(); // 0xfx07 stores the current value of the delay timer in vx
void _ch8_wait_key(); // 0xfx0a waits for a key press then stores the key value in vx
void _ch8_set_dt(); // 0xfx15 sets the delay timer to vx
void _ch8_set_st(); // 0xfx18 sets the sound timer to vx
void _ch8_addi(); // 0xfx1e sets the index register to ir + vx
void _ch8_load_fnt(); // 0xfx29 sets the index register to the location of the character vx in the fontset
void _ch8_load_bcd(); // 0xfx33 stores the bcd representation of vx into _ir, _ir + 1 and _ir + 2.
void _ch8_w_mem(); // 0xfx55 stores registers v0 through vx into memory starting at the index register
void _ch8_l_mem(); // 0xfx65 loads x bytes starting at the index register into registers v0 through vx

#endif
