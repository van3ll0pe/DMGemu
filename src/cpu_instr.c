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
    cpu_updateFlag(cpu, H_FLAG, ((data & 0xf) + 1) > 0xf);

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

void instr_scf(Cpu* cpu) {
    if (!cpu) {exit(1);}

    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, true);
}

void instr_ccf(Cpu* cpu) {
    if (!cpu) {exit(1);}

    uint8_t carry = cpu_getFlag(cpu, C_FLAG) ^ 1;

    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, carry);
}

void instr_cpl(Cpu* cpu) {
    if (!cpu) {exit(1);}

    cpu->AF.r8.hi = ~cpu->AF.r8.hi;

    cpu_updateFlag(cpu, N_FLAG, true);
    cpu_updateFlag(cpu, H_FLAG, true);
}

void instr_daa(Cpu* cpu) {
    if (!cpu) {exit(1);}

    if (cpu_getFlag(cpu, N_FLAG) == 0) { //addition
            if (cpu_getFlag(cpu, C_FLAG) == 1 || cpu->AF.r8.hi > 0x99) {
                cpu->AF.r8.hi += 0x60;
                cpu_updateFlag(cpu, C_FLAG, true);
            }
            if (cpu_getFlag(cpu, H_FLAG) == 1 || (cpu->AF.r8.hi & 0xf) > 0x09) {
                cpu->AF.r8.hi += 0x6;
            }
        }
        else { //soustraction
            if (cpu_getFlag(cpu, C_FLAG) == 1) {
                cpu->AF.r8.hi -= 0x60;
            }
            if (cpu_getFlag(cpu, H_FLAG) == 1) {
                cpu->AF.r8.hi -= 0x6;
            }
        }

        cpu_updateFlag(cpu, Z_FLAG, cpu->AF.r8.hi == 0);
        cpu_updateFlag(cpu, H_FLAG, false);
}

void instr_add16(Cpu* cpu, uint16_t r16) {
    if (!cpu) {exit(1);}

    uint16_t res = cpu->HL.r16 + r16;

    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, ((cpu->HL.r16 & 0xfff) + (r16 & 0xfff)) > 0xfff);
    cpu_updateFlag(cpu, C_FLAG, res <  cpu->HL.r16);

     cpu->HL.r16 = res;
}

uint16_t instr_add16imm(Cpu* cpu, int8_t e8) {
    if (!cpu) {exit(1);}

    cpu_updateFlag(cpu, Z_FLAG, false);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, ((cpu->SP & 0xf) + (e8 & 0xf)) > 0xf);
    cpu_updateFlag(cpu, C_FLAG, ((cpu->SP & 0xff) + (e8 & 0xff)) > 0xff);

    return (cpu->SP + e8);
}

void instr_add8(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = cpu->AF.r8.hi + data;

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, ((cpu->AF.r8.hi & 0xf) + (data & 0xf)) > 0xf);
    cpu_updateFlag(cpu, C_FLAG, (cpu->AF.r8.hi + data) > 0xff);

    cpu->AF.r8.hi = res;
}

void instr_adc(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t c = cpu_getFlag(cpu, C_FLAG);
    uint8_t res = cpu->AF.r8.hi + data + c;

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, ((cpu->AF.r8.hi & 0xf) + (data & 0xf) + c) > 0xf);
    cpu_updateFlag(cpu, C_FLAG, (cpu->AF.r8.hi + data + c) > 0xff);

    cpu->AF.r8.hi = res;
}

void instr_sub(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = cpu->AF.r8.hi - data;

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, true);
    cpu_updateFlag(cpu, H_FLAG, (cpu->AF.r8.hi & 0xf) < (data & 0xf));
    cpu_updateFlag(cpu, C_FLAG, cpu->AF.r8.hi < data);

    cpu->AF.r8.hi = res;
}

void instr_sbc(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t c = cpu_getFlag(cpu, C_FLAG);
    uint8_t res = cpu->AF.r8.hi - data - c;

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, true);
    cpu_updateFlag(cpu, H_FLAG, (cpu->AF.r8.hi & 0xf) < ((data & 0xf) + c));
    cpu_updateFlag(cpu, C_FLAG, cpu->AF.r8.hi < (data + c));

    cpu->AF.r8.hi = res;
}
void instr_and(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    cpu->AF.r8.hi &= data;

    cpu_updateFlag(cpu, Z_FLAG, cpu->AF.r8.hi == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, true); 
    cpu_updateFlag(cpu, C_FLAG, false);
}

void instr_xor(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    cpu->AF.r8.hi ^= data;

    cpu_updateFlag(cpu, Z_FLAG, cpu->AF.r8.hi == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, false);
}

void instr_or(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    cpu->AF.r8.hi |= data;

    cpu_updateFlag(cpu, Z_FLAG, cpu->AF.r8.hi == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, false);
}

void instr_cp(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = cpu->AF.r8.hi - data;

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, true);
    cpu_updateFlag(cpu, H_FLAG, (cpu->AF.r8.hi & 0xf) < (data & 0xf));
    cpu_updateFlag(cpu, C_FLAG, cpu->AF.r8.hi < data);
}

uint8_t instr_rlc(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t b7 = data & 0x80;
    uint8_t res = (data << 1) | ((b7) ? 1 : 0);

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, b7);

    return res;
}

uint8_t instr_rrc(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t b0 = data & 0x1;
    uint8_t res = (data >> 1) | ((b0) ? 0x80 : 0);

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, b0);

    return res;
}

uint8_t instr_rl(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t b7 = data & 0x80;
    uint8_t res = (data << 1) | cpu_getFlag(cpu, C_FLAG);

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, b7);

    return res;
}

uint8_t instr_rr(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t b0 = data & 0x1;
    uint8_t res = (data >> 1) | ((cpu_getFlag(cpu, C_FLAG)) ? 0x80: 0);

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, b0);

    return res;
}

uint8_t instr_sla(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = (data << 1);

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, (data & 0x80));

    return res;
}

uint8_t instr_sra(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = (data >> 1) | (data & 0x80);

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, (data & 0x1));

    return res;
}

uint8_t instr_swap(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = ((data & 0xf0) >> 4) | ((data & 0x0f) << 4);

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, false);

    return res;
}

uint8_t instr_srl(Cpu* cpu, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = (data >> 1);

    cpu_updateFlag(cpu, Z_FLAG, res == 0);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, false);
    cpu_updateFlag(cpu, C_FLAG, (data & 0x1));

    return res;
}

void instr_bit(Cpu* cpu, uint8_t bit, uint8_t data) {
    if (!cpu) {exit(1);}

    uint8_t res = (1 << bit) & data;

    cpu_updateFlag(cpu, Z_FLAG, !res);
    cpu_updateFlag(cpu, N_FLAG, false);
    cpu_updateFlag(cpu, H_FLAG, true);
}

uint8_t instr_res(uint8_t bit, uint8_t data) {
    return data & ~(1 << bit);
}

uint8_t instr_set(uint8_t bit, uint8_t data) {
    return data | (1 << bit);
}