#include "video.h"
#include "SDL/SDL.h"
#define RES_X 64
#define RES_Y 32
#define STRETCH_BY 10

void ch8_init_video()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("c8emu", NULL);
    _screen = SDL_SetVideoMode((RES_X * STRETCH_BY),(RES_Y * STRETCH_BY),16,SDL_HWSURFACE);
}

void ch8_close_video()
{
    atexit(SDL_Quit);
}

void ch8_video_draw_fb(unsigned char* fb)
{
    int x,y;
    SDL_LockSurface(_screen);
    for(y = 0; y < 32; y++)
    {
        for(x = 0; x < 64; x++)
        {
            if(fb[(y*64) + x]) // If pixel is set
            {
                _ch8_video_draw_pixel(x, y, (Uint16) 0xffff);
            }
            else
            {
                _ch8_video_draw_pixel(x, y, (Uint16) 0x0000);
            }
        }
    }
    SDL_UnlockSurface(_screen);
    SDL_UpdateRect(_screen,0,0,0,0);
}

void _ch8_video_draw_pixel(unsigned int x, unsigned int y, Uint16 color)
{
    Uint16 *pixels = _screen->pixels;
    int dx,dy;
    
    for(dy = 0; dy < STRETCH_BY; dy++)
    {
        for(dx = 0; dx < STRETCH_BY; dx++)
        {
            pixels[((y*STRETCH_BY) + dy) * (_screen->pitch/2) + (x*STRETCH_BY)+dx] = color;
        }
    }
}
