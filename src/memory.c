#include "memory.h"
#include <stdio.h>
#include <string.h>

void memory_init(Memory* memory)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    memset(memory->buffer, 0, sizeof(uint8_t) * MEMORY_SIZE);
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