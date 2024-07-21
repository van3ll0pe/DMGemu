#include <SDL2/SDL.h>
#include "gameboy.h"

int main(int ac, char** av)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "[Error] : initialization of SDL failed");
        exit(1);
    }

    Gameboy gameboy;
    gameboy_init(&gameboy);

    gameboy_run(&gameboy);

    SDL_Quit();
    return 0;
}