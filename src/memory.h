#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>

#define MEMORY_SIZE 0x10000

typedef struct {
    uint8_t buffer[MEMORY_SIZE];
    uint8_t bootRom[0x100];
} Memory;


void memory_init(Memory* memory);
uint8_t memory_read8(Memory* memory, uint16_t address);
void memory_write8(Memory* memory, uint16_t address, uint8_t data);
uint16_t memory_read16(Memory* memory, uint16_t address);
void memory_write16(Memory* memory,uint16_t address, uint16_t data);

#endif //__MEMORY_H__