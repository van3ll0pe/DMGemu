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
        exit(1);
    }

    FILE* cartridge = fopen(av[1], "rb");
    long size = 0;
    fseek(cartridge, 0, SEEK_END);
    size = ftell(cartridge);
    fseek(cartridge, 0, SEEK_SET);

    uint8_t *data_cartridge = malloc(sizeof(uint8_t)*size);
    fread(data_cartridge, 1, size, cartridge);
    fclose(cartridge);

    Gameboy gameboy;
    gameboy_init(&gameboy);
    gameboy_insert_cartridge(&gameboy, data_cartridge);
    gameboy_run(&gameboy);

    free(data_cartridge);
    SDL_Quit();
    return 0;
}