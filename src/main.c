#include <SDL2/SDL.h>
#include "gameboy.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int ac, char** av)
{

    if (ac == 1)
        return 1;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "[Error] : initialization of SDL failed");
        abort();
    }

    Gameboy gb;
    if (!gameboy_init(&gb, av[1])) {
        return 1;
    }
    gameboy_run(&gb);
    gameboy_quit(&gb);

    SDL_Quit();
    return 0;
}