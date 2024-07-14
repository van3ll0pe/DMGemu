#ifndef __CPU_INSTR_H__
#define __CPU_INSTR_H__

#include "cpu.h"
#include <stdint.h>

//arithmetic operation 8bits / 16bits
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
void cpu_instr_DAA(Cpu* cpu);

//load operation 8bits
void cpu_instr_LDr8_8(uint8_t* dst, uint8_t value);
void cpu_instr_LDmem16_8(Cpu* cpu, uint16_t address, uint8_t value);

//load operation 16bits
void cpu_instr_LDr16_16(uint16_t* dst, uint16_t value);
void cpu_instr_LDmem16_16(Cpu* cpu, uint16_t address, uint16_t value);
void cpu_instr_LDHLSPe8(Cpu* cpu, int8_t value);

//POP PUSH
void cpu_instr_PUSH(Cpu* cpu, uint16_t r16);
void cpu_instr_POP(Cpu* cpu, uint16_t* r16);

//FLAG instruction & others
void cpu_instr_CCF(Cpu* cpu);
void cpu_instr_SCF(Cpu* cpu);
void cpu_instr_CPL(Cpu* cpu);

//INTERRUPTS
void cpu_instr_EI(Cpu* cpu);
void cpu_instr_DI(Cpu* cpu);

//JUMPS
uint8_t cpu_instr_JP(Cpu* cpu, bool condition, uint16_t address); //returns the cycle machine
uint8_t cpu_instr_CALL(Cpu* cpu, bool condition, uint16_t address); //returns the cycle machine
uint8_t cpu_instr_RET(Cpu* cpu, bool condition);
uint8_t cpu_instr_JR(Cpu* cpu, bool condition, int8_t value); //returns the cycle machine
void cpu_instr_RST(Cpu* cpu, uint16_t address);

//BIT OPERATIONS
void cpu_instr_RLCA(Cpu* cpu);
void cpu_instr_RRCA(Cpu* cpu);
void cpu_instr_RLA(Cpu* cpu);
void cpu_instr_RRA(Cpu* cpu);

void cpu_instr_RLC(Cpu* cpu, uint8_t* r8);
void cpu_instr_RLCHL(Cpu* cpu, uint16_t address);
void cpu_instr_RRC(Cpu* cpu, uint8_t* r8);
void cpu_instr_RRCHL(Cpu* cpu, uint16_t address);
void cpu_instr_RL(Cpu* cpu, uint8_t* r8);
void cpu_instr_RLHL(Cpu* cpu, uint16_t address);
void cpu_instr_RR(Cpu* cpu, uint8_t* r8);
void cpu_instr_RRHL(Cpu* cpu, uint16_t address);
void cpu_instr_SLA(Cpu* cpu, uint8_t* r8);
void cpu_instr_SLAHL(Cpu* cpu, uint8_t* r8);
void cpu_instr_SRA(Cpu* cpu, uint8_t* r8);
void cpu_instr_SRAHL(Cpu* cpu, uint16_t address);
void cpu_instr_SRL(Cpu* cpu, uint8_t* r8);
void cpu_instr_SRLHL(Cpu* cpu, uint16_t address);
void cpu_instr_SWAP(Cpu* cpu, uint8_t* r8);
void cpu_instr_SWAPHL(Cpu* cpu, uint16_t address);
void cpu_instr_BIT(Cpu* cpu, BIT bit, uint8_t value);
void cpu_instr_RES(Cpu* cpu, BIT bit, uint8_t* r8);
void cpu_instr_RESHL(Cpu* cpu, BIT bit, uint16_t address);
void cpu_instr_SET(Cpu* cpu, BIT bit, uint8_t* r8);
void cpu_instr_SETHL(Cpu* cpu, BIT bit, uint16_t address);
#endif //__CPU_INSTR_H__