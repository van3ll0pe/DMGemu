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

uint8_t instr_inc8(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = data + 1;

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, (data & 0xf) + 1 > 0xf);

    return res;
}

uint8_t instr_dec8(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = data - 1;

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, true);
    cpu_updateFlag(cpu, H_FLAG, (data & 0xf) == 0);

    return res;
}