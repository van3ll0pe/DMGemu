#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t sb;
    uint8_t sc;
    uint8_t interrupt;
} Serial;

void serial_init(Serial* serial);
void serial_write(Serial* serial, uint16_t address, uint8_t data);
uint8_t serial_read(Serial* serial, uint16_t address);

#endif