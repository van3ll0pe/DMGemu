#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

void timer_init(Timer* timer) {
    if (!timer) {abort();}

    timer->clock_speed = 1024; //clock ticks number to increment tima
    timer->div = 0xAB;
    timer->div_cycles = 0;
    timer->tac = 0;
    timer->tima = 0;
    timer->tima_cycles = 0;
    timer->tma = 0;
    timer->enabled = false;
    timer->interrupt = 0;
}

uint8_t timer_read(Timer* timer, uint16_t address) {
    if (!timer) {abort();}

    switch (address) {
        case 0xFF04: { return timer->div; }
        case 0xFF05: { return timer->tima; }
        case 0xFF06: { return timer->tma; }
        case 0xFF07: { return timer->tac; }
        default : { fprintf(stderr, "Error: invalid address to read timer"); abort();}
    }
}
void timer_write(Timer* timer, uint16_t address, uint8_t data) {
    if (!timer) {abort();}

    switch(address) {
        case 0xFF04: { timer->div = 0; return; }
        case 0xFF05: { timer->tima = data; return; }
        case 0xFF06: { timer->tma = data; return; }
        case 0xFF07: { timer->tac = (data | 0xF8); timer->enabled = (data & 0x4) != 0; switch (data & 0x3) { //0xF8 mask is to put useless bit to 1 and only get useful bit from data
                                                                        case 0x0: {timer->clock_speed = 1024; break; }
                                                                        case 0x1: {timer->clock_speed = 16; break; }
                                                                        case 0x2: {timer->clock_speed = 64; break; }
                                                                        case 0x3: {timer->clock_speed = 256; break; }
                                                                        default: {abort();}; }
                    return;
                    }
        default : {fprintf(stderr, "Error : invalid address to write timer"); abort();}
    }
}

void timer_ticks(Timer* timer, uint32_t ticks) {
    if (!timer) {abort();}

    timer->div_cycles += ticks;

    while( timer->div_cycles >= 256) {
        timer->div++;
        timer->div_cycles -= 256;
    }

    if (timer->enabled == false) { return ;}

    timer->tima_cycles += ticks;
    while (timer->tima_cycles >= timer->clock_speed) {
        timer->tima++;
        if (timer->tima == 0x00) {
            timer->tima = timer->tma;
            timer->interrupt = 0x4;
        }
        timer->tima_cycles -= timer->clock_speed;
    }
}