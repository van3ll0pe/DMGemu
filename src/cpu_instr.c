#include "cpu.h"

/**************** 8bit arithmetic and logic instructions **********************/

void cpu_instr_ADC(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);
    
   uint32_t res = cpu->AF.r8.hi + value + cpu_getFlag(cpu, C_FLAG);
   uint8_t resFinal = (uint8_t)res;

    cpu_checkFlag(cpu, Z_FLAG, (resFinal == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->AF.r8.hi & 0xF) + (value & 0xF) + cpu_getFlag(cpu, C_FLAG)) & 0x10));
    cpu_checkFlag(cpu, C_FLAG, (res & 0x100));

    cpu->AF.r8.hi = resFinal;
}

void cpu_instr_ADD8(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);

    uint32_t res = cpu->AF.r8.hi + value;
    uint8_t resFinal = (uint8_t)res;

    cpu_checkFlag(cpu, Z_FLAG, (resFinal == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->AF.r8.hi & 0xF) + (value & 0xF)) & 0x10));
    cpu_checkFlag(cpu, C_FLAG, (res & 0x100));

    cpu->AF.r8.hi = resFinal;
}

void cpu_instr_ADD16(Cpu* cpu, uint16_t value)
{
    if (!cpu)
        exit(1);
    
    uint32_t res = cpu->HL.r16 + value;
    uint16_t resFinal = (uint16_t)res;

    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->HL.r16 & 0xFFF) + (value & 0xFFF)) & 0x1000));
    cpu_checkFlag(cpu, C_FLAG, (res & 0x10000));

    cpu->HL.r16 = resFinal;
}

void cpu_instr_ADDe8(Cpu* cpu, uint8_t value);



