#ifndef __GAMEBOY_H__
#define __GAMEBOY_H__

#include "cpu.h"
#include "memory.h"
#include "joypad.h"
#include "serial.h"
#include "timer.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

typedef struct {
    Cpu cpu;
    Memory memory;
    Joypad joypad;
    Serial serial;
    Timer timer;

    SDL_Window* window;
    SDL_Renderer* render;
    SDL_Texture* texture;
} Gameboy;

bool gameboy_init(Gameboy* gb);
bool gameboy_draw(Gameboy* gb);
bool gameboy_run(Gameboy* gb);

#endif