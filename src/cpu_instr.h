#ifndef __CPU_INSTR_H__
#define __CPU_INSTR_H__

#include "cpu.h"
#include <stdint.h>

uint16_t instr_pop(Cpu* cpu);
void instr_push(Cpu* cpu, uint16_t r16);

void instr_dec16(uint16_t* r16);
void instr_inc16(uint16_t* r16);
uint8_t instr_inc8(Cpu* cpu, uint8_t data);
uint8_t instr_dec8(Cpu* cpu, uint8_t data);
void instr_scf(Cpu* cpu);
void instr_ccf(Cpu* cpu);
void instr_cpl(Cpu* cpu);
void instr_daa(Cpu* cpu);
void instr_add16(Cpu* cpu, uint16_t r16);
uint16_t instr_add16imm(Cpu* cpu, int8_t e8);
void instr_add8(Cpu* cpu, uint8_t data);
void instr_adc(Cpu* cpu, uint8_t data);
void instr_sub(Cpu* cpu, uint8_t data);
void instr_sbc(Cpu* cpu, uint8_t data);
void instr_and(Cpu* cpu, uint8_t data);
void instr_xor(Cpu* cpu, uint8_t data);
void instr_or(Cpu* cpu, uint8_t data);
void instr_cp(Cpu* cpu, uint8_t data);

uint8_t instr_rlc(Cpu* cpu, uint8_t data);
uint8_t instr_rrc(Cpu* cpu, uint8_t data);
uint8_t instr_rl(Cpu* cpu, uint8_t data);
uint8_t instr_rr(Cpu* cpu, uint8_t data);
uint8_t instr_sla(Cpu* cpu, uint8_t data);
uint8_t instr_sra(Cpu* cpu, uint8_t data);
uint8_t instr_swap(Cpu* cpu, uint8_t data);
uint8_t instr_srl(Cpu* cpu, uint8_t data);
void instr_bit(Cpu* cpu, uint8_t bit, uint8_t data);
uint8_t instr_res(uint8_t bit, uint8_t data);
uint8_t instr_set(uint8_t bit, uint8_t data);
void instr_jr(Cpu* cpu, int8_t e8);
#endif //__CPU_INSTR_H__