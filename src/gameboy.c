#include "gameboy.h"

bool gameboy_init(Gameboy* gb, const char* filename) {
    if (!gb) { return false; }

    load_cartridge(&gb->cartridge, filename);
    timer_init(&gb->timer);
    serial_init(&gb->serial);
    joypad_init(&gb->joypad);
    memory_init(&gb->memory, &gb->serial, &gb->timer, &gb->joypad, &gb->cartridge);
    cpu_init(&gb->cpu, &gb->memory);

    gb->window = SDL_CreateWindow("DMGemu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160, 144, SDL_WINDOW_SHOWN);
    if (!gb->window) { return false; }
    gb->render = SDL_CreateRenderer(gb->window, -1, SDL_RENDERER_ACCELERATED);
    if (!gb->render) { SDL_DestroyWindow(gb->window); return false; }

    return true;
}

bool gameboy_draw(Gameboy* gb) {
    return true;
}

void gameboy_run(Gameboy* gb) {
    while (!gb->joypad.exit_gameboy) {
        uint32_t ticks = cpu_ticks(&gb->cpu);

        gb->memory.interrupt_requested |= gb->serial.interrupt;
        gb->serial.interrupt = 0;

        timer_ticks(&gb->timer, ticks);
        gb->memory.interrupt_requested |= gb->timer.interrupt;
        gb->timer.interrupt = 0;

        get_event(&gb->joypad);
        gb->memory.interrupt_requested |= gb->joypad.interrupt;
        gb->joypad.interrupt = 0;
        SDL_Delay(60);
    }
}

void gameboy_quit(Gameboy* gb) {
    SDL_DestroyRenderer(gb->render);
    SDL_DestroyWindow(gb->window);
    eject_cartridge(&gb->cartridge);
}