#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;

    uint32_t clock_speed;
    bool enabled;

    uint32_t div_cycles;
    uint32_t tima_cycles;

    uint8_t interrupt;
} Timer;

void timer_init(Timer* timer);
uint8_t timer_read(Timer* timer, uint16_t address);
void timer_write(Timer* timer, uint16_t address, uint8_t data);

void timer_ticks(Timer* timer, uint32_t ticks);

#endif