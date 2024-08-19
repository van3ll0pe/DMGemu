#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

void timer_init(Timer* timer) {
    if (!timer) {exit(1);}

    timer->clock_speed = 256 * 4; //default clock increment div in clock cycles
    timer->div = 0;
    timer->div_cycles = 0;
    timer->tac = 0;
    timer->tima = 0;
    timer->tima_cycles = 0;
    timer->tma = 0;
    timer->enabled = false;
    timer->interrupt_request = false;
}

uint8_t timer_read(Timer* timer, uint16_t address) {
    if (!timer) {exit(1);}

    switch (address) {
        case 0xFF04: { return timer->div; }
        case 0xFF05: { return timer->tima; }
        case 0xFF06: { return timer->tma; }
        case 0xFF07: { return timer->tac; }
        default : { fprintf(stderr, "Error: invalid address to read timer"); exit(1);}
    }
}
void timer_write(Timer* timer, uint16_t address, uint8_t data) {
    if (!timer) {exit(1);}

    switch(address) {
        case 0xFF04: { timer->div = 0; break; }
        case 0xFF05: { timer->tima = data; break; }
        case 0xFF06: { timer->tma = data; break; }
        case 0xFF07: { timer->tac = data; timer->enabled = data & 0x4; switch (data & 0x3) {
                                                                        case 0x0: {timer->clock_speed = 256 * 4; break; }
                                                                        case 0x1: {timer->clock_speed = 4 * 4; break; }
                                                                        case 0x2: {timer->clock_speed = 16 * 4; break; }
                                                                        case 0x3: {timer->clock_speed = 64 * 4; break; }
                                                                        default: {exit(1);}}
                    }
        default : {fprintf(stderr, "Error : invalid address to write timer"); exit(1);}
    }
}

void timer_ticks(Timer* timer, uint32_t ticks) {
    if (!timer) {exit(1);}

    timer->div_cycles += ticks;
    while( timer->div_cycles >= 256 * 4) {
        timer->div++;
        timer->div_cycles -= (256 * 4);
    }

    if (timer->enabled == false) { return ;}

    timer->tima_cycles += ticks;
    while (timer->tima_cycles >= timer->clock_speed) {
        timer->tima++;
        if (timer->tima == 0x00) {
            timer->tima = timer->tma;
            timer->interrupt_request = true;
        }
        timer->tima_cycles -= timer->clock_speed;
    }
}