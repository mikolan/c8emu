#include "video.h"
#include "SDL/SDL.h"

void ch8_init_video()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("c8emu", NULL);
    SDL_SetVideoMode(640,320,16,SDL_HWSURFACE);
}

void ch8_draw_fb(char** fb)
{

}
