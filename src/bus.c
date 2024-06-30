#include "bus.h"
#include <stdio.h>
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
    bus->bus_read16 = &memory_read16;
    bus->bus_read8 = &memory_read8;
    bus->bus_write16 = &memory_write16;
    bus->bus_write8 = &memory_write8;
}