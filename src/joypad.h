#ifndef __JOYPAD_H__
#define __JOYPAD_H__

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct  {
    uint8_t p1;
    bool dpad_activated;
    bool buttons_activated;
    uint8_t buttons; //bit 7 = start; bit 6 = select; bit 5 = B; bit 4 = A; bit 3 = down; bit 2 = up; bit 1 = left; bit 0 = right 
    bool exit_gameboy;
    bool interrupt_request;
}Joypad;

void joypad_init(Joypad* joypad);
void joypad_write(Joypad* joypad, uint16_t address, uint8_t data);
uint8_t joypad_read(Joypad* joypad, uint16_t address);
void joypad_keydown(Joypad* joypad, uint8_t button);
void joypad_keyup(Joypad* joypad, uint8_t button);
uint8_t get_associate_key(SDL_Keycode key);
void get_event(Joypad* joypad);
void joypad_update(Joypad* joypad);

#endif