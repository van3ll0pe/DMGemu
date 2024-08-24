#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include "timer.h"
#include "serial.h"
#include "joypad.h"

#define WORKRAM_SIZE 0x2000
#define HIGHRAM_SIZE 0x7F
#define OAMRAM_SIZE 0xA0

typedef struct {
    uint8_t interrupt_requested; //IF - FF0F
    uint8_t interrupt_enable; //IE - FFFF
    uint8_t disable_bootrom; //FF50

    uint8_t work_ram[WORKRAM_SIZE];
    uint8_t high_ram[HIGHRAM_SIZE];
    uint8_t oam_ram[OAMRAM_SIZE];
    Timer* timer;
    Serial* serial;
    Joypad* joypad;

} Memory;


void memory_init(Memory* memory, Serial* serial, Timer* timer, Joypad* joypad);
uint8_t memory_read8(Memory* memory, uint16_t address);
void memory_write8(Memory* memory, uint16_t address, uint8_t data);
uint16_t memory_read16(Memory* memory, uint16_t address);
void memory_write16(Memory* memory,uint16_t address, uint16_t data);

#endif //__MEMORY_H__