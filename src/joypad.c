#include "joypad.h"
#include <SDL2/SDL.h>

void joypad_init(Joypad* joypad)
{
    if (!joypad)
        exit(1);
    
    memset(joypad, false, sizeof(Joypad));
}

void joypad_buttonPressed(bool* button)
{
    if (!button)
        return;

    *button = true;
}

void joypad_buttonReleased(bool* button)
{
    if (!button)
        return;
    
    *button = false;
}

bool* joypad_getAssociatedKey(Joypad* joypad, int key)
{
    if (!joypad)
        exit(1);
    
    switch(key) {
        case SDLK_UP: return &joypad->button_up;
        case SDLK_DOWN: return &joypad->button_down;
        case SDLK_LEFT: return &joypad->button_left;
        case SDLK_RIGHT: return &joypad->button_right;
        case SDLK_e: return &joypad->button_a;
        case SDLK_r: return &joypad->button_b;
        case SDLK_d: return &joypad->button_start;
        case SDLK_f: return &joypad->button_select;
        default: return NULL;
    }
}

void joypad_updateP1(Bus* bus, Joypad* joypad)
{
    if (!joypad || !bus)
        exit(1);
    
    uint8_t hr_p1 = bus->bus_read8(bus->component, P1) | 0xCF;
    uint8_t requested_button = !(hr_p1 & 0x20);
    uint8_t requested_dpad = !(hr_p1 & 0x10);

    if ((requested_button && joypad->button_a) || (requested_dpad && joypad->button_right))
        hr_p1 &= ~0x01;
    if ((requested_button && joypad->button_b) || (requested_dpad && joypad->button_left))
        hr_p1 &= ~0x02;
    if ((requested_button && joypad->button_select) || (requested_dpad && joypad->button_up))
        hr_p1 &= ~0x04;
    if ((requested_button && joypad->button_start) || (requested_dpad && joypad->button_down))
        hr_p1 &= ~0x08;
    
    bus->bus_write8(bus->component, P1, hr_p1);
}

void get_event(Bus* bus, Joypad* joypad)
{
    if (!joypad || !bus)
        exit(1);
    
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) { //key pressed
            if (event.key.repeat)
                break;
            joypad_buttonPressed(joypad_getAssociatedKey(joypad, event.key.keysym.sym));
        }
        if (event.type == SDL_KEYUP) { //key released
            joypad_buttonReleased(joypad_getAssociatedKey(joypad, event.key.keysym.sym));
        }
        if (event.type == SDL_QUIT)
            joypad->quit = true;
    }

    joypad_updateP1(bus, joypad);
}