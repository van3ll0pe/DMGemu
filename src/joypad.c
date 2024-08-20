#include "joypad.h"
#include <stdio.h>
#include <stdlib.h>

void joypad_init(Joypad* joypad) {
    if (!joypad) {exit(1);}

    joypad->buttons = 0xff;
    joypad->dpad_activated = true;
    joypad->buttons_activated = true;
    joypad->p1 = 0xcf;
    joypad->exit_gameboy = false;
    joypad->interrupt_request = false;
}

uint8_t joypad_read(Joypad* joypad, uint16_t address) {
    if (!joypad) {exit(1);}

    if (address == 0xFF00) {
        return joypad->p1;
    } else {
        fprintf(stderr, "Error: invalid address for joypad");
        exit(1);
    }
}

void joypad_write(Joypad* joypad, uint16_t address, uint8_t data) {
    if (!joypad) {exit(1);}

    if (address != 0xFF00) {
        fprintf(stderr, "Error: invalid address for joypad");
        exit(1);
    }

    joypad->p1 = (joypad->p1 & 0xcf) | (data & 0x30);
    if ((data & 0x20) == 0) { joypad->buttons_activated = true; } else { joypad->buttons_activated = false; }
    if ((data & 0x10) == 0) { joypad->dpad_activated = true; } else { joypad->dpad_activated = false; }
    //we only take bit 4 and bit 5 from data and keep the other bit from p1 because low nybble is readonly and bit6 and bit7 are useless so set 1
    joypad_update(joypad);
}

uint8_t get_associate_key(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP: { return 0x04; } //arrow up
        case SDLK_DOWN: { return 0x08; } //arrow down
        case SDLK_RIGHT: { return 0x01; } //arrow right
        case SDLK_LEFT: { return 0x02; } //arrow left
        case SDLK_z: { return 0x10; } //button A
        case SDLK_e: { return 0x20; } //button B
        case SDLK_s: { return 0x40; } //button select
        case SDLK_d: { return 0x80; } //button start
        default : { return 0; }
    }
}

void joypad_keydown(Joypad* joypad, uint8_t button) {
    if (!joypad) {exit(1);}

    joypad->buttons &= ~(button); //is key pressed, bit set to 0
}

void joypad_keyup(Joypad* joypad, uint8_t button) {
    if (!joypad) {exit(1);}

    joypad->buttons |= button; //is key released, bit set to 1
}

void get_event(Joypad* joypad) {
    if (!joypad) {exit(1);}

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) { joypad->exit_gameboy = true; return; }
        if (event.type == SDL_KEYDOWN) { uint8_t key = get_associate_key(event.key.keysym.sym); if (key != 0) { joypad_keydown(joypad, key); joypad_update(joypad); } }
        if (event.type == SDL_KEYUP) { uint8_t key = get_associate_key(event.key.keysym.sym); if (key != 0) { joypad_keyup(joypad, key); joypad_update(joypad); } }
    }
}

void joypad_update(Joypad* joypad) {
    if (!joypad) {exit(1);}

    uint8_t current_state = joypad->p1 & 0x0f; //get current state buttons (low nybble)
    uint8_t new_state = 0x0f;

    if (joypad->buttons_activated == true) {
        new_state &= ((joypad->buttons >> 4) & 0xf);
    }

    if (joypad->dpad_activated == true) {
        new_state &= (joypad->buttons & 0xf);
    }

    if ( (current_state & ~(new_state)) != 0) {//request interrupt key pressed
        joypad->interrupt_request = true;
    }

    joypad->p1 = (joypad->p1 & 0xf0) | (new_state & 0xf);
}


