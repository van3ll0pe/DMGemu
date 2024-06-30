#include "cpu.h"
#include <stdio.h>

void cpu_init(Cpu* cpu)
{
    if (!cpu) {
        fprintf(stderr, "[ERROR]: CPU not initialized");
        exit(1);
    }

    cpu->bus = NULL;
    cpu->PC = 0x100;
    cpu->SP = 0xFFFE;

    cpu->AF.r8.lo = 0xB0; //reg F
    cpu->AF.r8.hi = 0x01; //reg A
    cpu->BC.r8.lo = 0x13; //reg C
    cpu->BC.r8.hi = 0x00; //reg B
    cpu->DE.r8.lo = 0xD8; //reg E
    cpu->DE.r8.hi = 0x00; //reg D
    cpu->HL.r8.lo = 0x4D; //reg L
    cpu->HL.r8.hi = 0x01; //reg H

    cpu->cycle = 0;
    cpu->opcode = 0;
}

void cpu_link_bus(Cpu* cpu, Bus* bus)
{
    if (!cpu) {
        fprintf(stderr, "[ERROR]: CPU is missing for link");
        exit(1);
    }

    if (!bus) {
        fprintf(stderr, "[ERROR]: BUS is missing for the link");
        exit(1);
    }

    cpu->bus = bus;
}

void cpu_setFlag(Cpu* cpu, uint8_t flag)
{
    if (!cpu)
        exit(1);
    
    cpu->AF.r8.lo |= flag;
}

void cpu_clearFlag(Cpu* cpu, uint8_t flag)
{
    if (!cpu)
        exit(1);
    
    cpu->AF.r8.lo &= (~flag);
}

void cpu_checkFlagN(Cpu* cpu)
{

}

void cpu_checkFlagZ(Cpu* cpu)
{

}

void cpu_checkFlagH(Cpu* cpu)
{

}

void cpu_checkFlagC(Cpu* cpu)
{

}