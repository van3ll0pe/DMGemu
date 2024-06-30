#ifndef __BUS_H__
#define __BUS_H__

#include <stdint.h>

typedef struct {
    void* component;
    uint8_t (*bus_read8)(void* component, uint16_t address);
    void (*bus_write8)(void* component, uint16_t address, uint8_t data);
    uint16_t (*bus_read16)(void* component, uint16_t address);
    void (*bus_write16)(void* component,uint16_t address, uint16_t data);
}Bus;

void bus_init(Bus* bus, void* componant);

#endif //__BUS_H__