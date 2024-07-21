#include "bus.h"
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

void bus_init(Bus* bus, void* component)
{
    if (!bus) {
        fprintf(stderr, "[ERROR]: bus is NULL for initialization");
        exit(1);
    }

    if (!component) {
        fprintf(stderr, "[ERROR]: Componant is not given to Bus");
        exit(0);
    }

    bus->component = component;
    bus->bus_read16 = (uint16_t (*)(void*, uint16_t))memory_read16;
    bus->bus_read8 = (uint8_t (*)(void*, uint16_t))memory_read8;
    bus->bus_write16 = (void (*)(void*, uint16_t, uint16_t))memory_write16;
    bus->bus_write8 = (void (*)(void*, uint16_t, uint8_t))memory_write8;
}