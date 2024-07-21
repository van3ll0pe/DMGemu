#include "memory.h"
#include "hard_registers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void memory_init(Memory* memory)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    memset(memory->buffer, 0, sizeof(uint8_t) * MEMORY_SIZE);

    memory->buffer[P1] = 0xCF;
    memory->buffer[SC] = 0x7E;
    memory->buffer[DIV] = 0xAB;
    memory->buffer[TAC] = 0xF8;
    memory->buffer[IF]  = 0xE1;
    memory->buffer[NR10] = 0x80;
    memory->buffer[NR11] = 0xBF;
    memory->buffer[NR12] = 0xF3;
    memory->buffer[NR13] = 0xFF;
    memory->buffer[NR14] = 0xBF;
    memory->buffer[NR21] = 0x3F;
    memory->buffer[NR23] = 0xFF;
    memory->buffer[NR24] = 0xBF;
    memory->buffer[NR30] = 0x7F;
    memory->buffer[NR31] = 0xFF;
    memory->buffer[NR32] = 0x9F;
    memory->buffer[NR33] = 0xFF;
    memory->buffer[NR34] = 0xBF;
    memory->buffer[NR41] = 0xFF;
    memory->buffer[NR44] = 0xBF;
    memory->buffer[NR50] = 0x77;
    memory->buffer[NR51] = 0xF3;
    memory->buffer[NR52] = 0xF1;
    memory->buffer[LCDC] = 0x91;
    memory->buffer[STAT] = 0x85;
    memory->buffer[DMA] = 0xFF;
    memory->buffer[BGP] = 0xFC;
    
}

uint8_t memory_read8(Memory* memory, uint16_t address)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    return memory->buffer[address];
}

void memory_write8(Memory* memory, uint16_t address, uint8_t data)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    memory->buffer[address] = data;
}

uint16_t memory_read16(Memory* memory, uint16_t address)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    return *(uint16_t*)(&memory->buffer[address]); //little endian extraction data
}

void memory_write16(Memory* memory,uint16_t address, uint16_t data)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    *(uint16_t*)(&memory->buffer[address]) = data; //little endian setup data
}