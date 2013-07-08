#ifndef CHIP8_VIDEO_H_
#define CHIP8_VIDEO_H_
#include "SDL/SDL.h"

SDL_Surface *_screen;

void ch8_init_video();
void ch8_close_video();
void ch8_video_draw_fb(unsigned char* fb);
void _ch8_video_draw_pixel(unsigned int x, unsigned int y, Uint16 color);

#endif
