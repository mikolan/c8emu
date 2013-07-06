#include "video.h"
#include "SDL.h"

void c8_init_video()
{
    SDL_init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("c8emu", NULL);
    SDL_SetVideoMode(640,320,16,SDL_HWSURFACE);
}

void c8_draw_fb(char** fb)
{

}
