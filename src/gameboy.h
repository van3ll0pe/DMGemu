#ifndef __GAMEBOY_H__
#define __GAMEBOY_H__

#include "bus.h"
#include "cpu.h"
#include "memory.h"
#include "timer.h"
#include "joypad.h"

typedef struct {
    Memory memory;
    Bus bus;
    Cpu cpu;
    Joypad joypad;
}Gameboy;

void gameboy_init(Gameboy* gameboy);
void gameboy_run(Gameboy* gameboy);

#endif