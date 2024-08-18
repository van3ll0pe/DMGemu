#include "cpu.h"
#include "cpu_instr.h"

uint16_t instr_pop(Cpu* cpu) {
    if (!cpu) { exit(1); }

    uint16_t r16 = memory_read16(cpu->bus, cpu->SP);
    cpu->SP += 2;

    return r16;
}

void instr_push(Cpu* cpu, uint16_t r16) {
    if (!cpu) { exit(1); }

    cpu->SP -= 2;
    memory_write16(cpu->bus, cpu->SP, r16);
}

void instr_dec16(uint16_t* r16) {
    if (!r16) { exit(1); }

    (*r16)--;
}

void instr_inc16(uint16_t* r16) {
    if (!r16) { exit(1); }

    (*r16)++;
}