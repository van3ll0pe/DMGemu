#ifndef __CPU_INSTR_H__
#define __CPU_INSTR_H__

#include "cpu.h"
#include <stdint.h>

void cpu_instr_ADC(Cpu* cpu, uint8_t value);
void cpu_instr_ADD8(Cpu* cpu, uint8_t value);
void cpu_instr_ADD16(Cpu* cpu, uint16_t value);
void cpu_instr_ADDe8(Cpu* cpu, int8_t value);
void cpu_instr_AND(Cpu* cpu, uint8_t value);
void cpu_instr_CP(Cpu* cpu, uint8_t value);
void cpu_instr_DEC8(Cpu* cpu, uint8_t* value);
void cpu_instr_DECHL(Cpu* cpu, uint16_t address);
void cpu_instr_DEC16(Cpu* cpu, uint16_t* value);
void cpu_instr_INC8(Cpu* cpu, uint8_t* value);
void cpu_instr_INCHL(Cpu* cpu, uint16_t address);
void cpu_instr_INC16(Cpu* cpu, uint16_t *value);
void cpu_instr_OR(Cpu* cpu, uint8_t value);
void cpu_instr_SBC(Cpu* cpu, uint8_t value);
void cpu_instr_SUB(Cpu* cpu, uint8_t value);
void cpu_instr_XOR(Cpu* cpu, uint8_t value);

#endif //__CPU_INSTR_H__