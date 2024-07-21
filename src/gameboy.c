#include "gameboy.h"
#include <stdlib.h>

void gameboy_init(Gameboy* gameboy)
{
    if (!gameboy)
        exit(1);
    
    memory_init(&gameboy->memory);
    bus_init(&gameboy->bus, &gameboy->memory);
    cpu_init(&gameboy->cpu);
    cpu_link_bus(&gameboy->cpu, &gameboy->bus);
    joypad_init(&gameboy->joypad);
}

void gameboy_run(Gameboy* gameboy)
{
    while(!gameboy->joypad.quit) {
        get_event(&gameboy->bus, &gameboy->joypad);     //get input form the machine
        handle_interrupts(&gameboy->cpu);               //get interruptions
        cpu_tick(&gameboy->cpu);                        //execute instruction of cpu
        //ppu_tick()
        handle_timer(&gameboy->bus, gameboy->cpu.cycle);//update timer in memory
    }
}