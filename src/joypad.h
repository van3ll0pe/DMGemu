#ifndef __JOYPAD_H__
#define __JOYPAD_H__

#include <stdbool.h>

#include "bus.h"
#include "hard_registers.h"

typedef struct {
    bool button_a;
    bool button_b;
    bool button_start;
    bool button_select;
    bool button_up;
    bool button_down;
    bool button_left;
    bool button_right;

    bool quit;
}Joypad;

//when button is true (is pressed), is false (not pressed)
void joypad_init(Joypad* joypad);
void joypad_buttonPressed(bool* button);
void joypad_buttonReleased(bool* button);
bool* joypad_getAssociatedKey(Joypad* joypad, int Key);

void get_event(Bus* bus, Joypad* joypad);

void joypad_updateP1(Bus* bus, Joypad* joyad);

#endif