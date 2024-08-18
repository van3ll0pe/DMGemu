#ifndef __CPU_INSTR_H__
#define __CPU_INSTR_H__

#include "cpu.h"
#include <stdint.h>

uint16_t instr_pop(Cpu* cpu);
void instr_push(Cpu* cpu, uint16_t r16);

void instr_dec16(uint16_t* r16);
void instr_inc16(uint16_t* r16);

#endif //__CPU_INSTR_H__