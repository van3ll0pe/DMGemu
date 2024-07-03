#include "cpu.h"
#include <stdio.h>
#include "cpu_instr.h"

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

void cpu_checkFlag(Cpu* cpu, uint8_t flag, bool condition)
{
    if (!cpu)
        exit(1);
    
    if (condition)
        cpu_setFlag(cpu, flag);
    else
        cpu_clearFlag(cpu, flag);
}

uint8_t cpu_getFlag(Cpu* cpu, uint8_t flag)
{
    if (!cpu)
        exit(1);
    
    return (cpu->AF.r8.lo & flag) ? 1 : 0;
}

//like bus_read8 but read data from PC and increment it
uint8_t cpu_getPCImm8(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint8_t data = cpu->bus->bus_read8(cpu->bus->component, cpu->PC);
    cpu->PC++;
    return data;
}

//like bus_read16 but read data from PC and increment it +2
uint16_t cpu_getPCImm16(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint16_t data = cpu->bus->bus_read16(cpu->bus->component, cpu->PC);
    cpu->PC += 2;
    return data;
}

void cpu_fetch_instruction(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    cpu->opcode = cpu->bus->bus_read8(cpu->bus->component, cpu->PC);
    cpu->PC++;
}

void cpu_execute_instruction(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    switch (cpu->opcode) {
        case 0x00: break;
        case 0x01: break;
        case 0x02: break;
        case 0x03: break;
        case 0x04: break;
        case 0x05:  //DEC B
                    cpu_instr_DEC8(cpu, &cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x06: break;
        case 0x07: break;
        case 0x08: break;
        case 0x09:  //ADD HL, BC
                    cpu_instr_ADD16(cpu, cpu->BC.r16);
                    cpu->cycle = 8;
                    break;
        case 0x0A: break;
        case 0x0B:  //DEC BC
                    cpu_instr_DEC16(cpu, &cpu->BC.r16);
                    cpu->cycle = 8;
                    break;
        case 0x0C: break;
        case 0x0D:  //DEC C
                    cpu_instr_DEC8(cpu, &cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x0E: break;
        case 0x0F: break;
        
        case 0x10: break;
        case 0x11: break;
        case 0x12: break;
        case 0x13: break;
        case 0x14: break;
        case 0x15:  //DEC D
                    cpu_instr_DEC8(cpu, &cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x16: break;
        case 0x17: break;
        case 0x18: break;
        case 0x19:  //ADD HL, DE
                    cpu_instr_ADD16(cpu, cpu->DE.r16);
                    cpu->cycle = 8;
                    break;
        case 0x1A: break;
        case 0x1B:  //DEC DE
                    cpu_instr_DEC16(cpu, &cpu->DE.r16);
                    cpu->cycle = 8;
                    break;
        case 0x1C: break;
        case 0x1D:  //DEC E
                    cpu_instr_DEC8(cpu, &cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x1E: break;
        case 0x1F: break;

        case 0x20: break;
        case 0x21: break;
        case 0x22: break;
        case 0x23: break;
        case 0x24: break;
        case 0x25:  //DEC H
                    cpu_instr_DEC8(cpu, &cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x26: break;
        case 0x27: break;
        case 0x28: break;
        case 0x29:  //ADD HL, HL
                    cpu_instr_ADD16(cpu, cpu->HL.r16);
                    cpu->cycle = 8;
                    break;
        case 0x2A: break;
        case 0x2B:  //DEC HL
                    cpu_instr_DEC16(cpu, &cpu->HL.r16);
                    cpu->cycle = 8;
                    break;
        case 0x2C: break;
        case 0x2D:  //DEC L
                    cpu_instr_DEC8(cpu, &cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x2E: break;
        case 0x2F: break;

        case 0x30: break;
        case 0x31: break;
        case 0x32: break;
        case 0x33: break;
        case 0x34: break;
        case 0x35:  //DEC [HL]
                    cpu_instr_DECHL(cpu, cpu->HL.r16);
                    cpu->cycle = 12;
                    break;
        case 0x36: break;
        case 0x37: break;
        case 0x38: break;
        case 0x39:  //ADD HL, SP
                    cpu_instr_ADD16(cpu, cpu->SP);
                    cpu->cycle = 8;
                    break;
        case 0x3A: break;
        case 0x3B:  //DEC SP
                    cpu_instr_DEC16(cpu, &cpu->SP);
                    cpu->cycle = 8;
                    break;
        case 0x3C: break;
        case 0x3D:  //DEC A
                    cpu_instr_DEC8(cpu, &cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x3E: break;
        case 0x3F: break;

        case 0x40: break;
        case 0x41: break;
        case 0x42: break;
        case 0x43: break;
        case 0x44: break;
        case 0x45: break;
        case 0x46: break;
        case 0x47: break;
        case 0x48: break;
        case 0x49: break;
        case 0x4A: break;
        case 0x4B: break;
        case 0x4C: break;
        case 0x4D: break;
        case 0x4E: break;
        case 0x4F: break;

        case 0x50: break;
        case 0x51: break;
        case 0x52: break;
        case 0x53: break;
        case 0x54: break;
        case 0x55: break;
        case 0x56: break;
        case 0x57: break;
        case 0x58: break;
        case 0x59: break;
        case 0x5A: break;
        case 0x5B: break;
        case 0x5C: break;
        case 0x5D: break;
        case 0x5E: break;
        case 0x5F: break;

        case 0x60: break;
        case 0x61: break;
        case 0x62: break;
        case 0x63: break;
        case 0x64: break;
        case 0x65: break;
        case 0x66: break;
        case 0x67: break;
        case 0x68: break;
        case 0x69: break;
        case 0x6A: break;
        case 0x6B: break;
        case 0x6C: break;
        case 0x6D: break;
        case 0x6E: break;
        case 0x6F: break;

        case 0x70: break;
        case 0x71: break;
        case 0x72: break;
        case 0x73: break;
        case 0x74: break;
        case 0x75: break;
        case 0x76: break;
        case 0x77: break;
        case 0x78: break;
        case 0x79: break;
        case 0x7A: break;
        case 0x7B: break;
        case 0x7C: break;
        case 0x7D: break;
        case 0x7E: break;
        case 0x7F: break;

        case 0x80:  //ADD A, B
                    cpu_instr_ADD8(cpu, cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x81:  //ADD A, C
                    cpu_instr_ADD8(cpu, cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x82:  //ADD A, D
                    cpu_instr_ADD8(cpu, cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x83:  //ADD A, E
                    cpu_instr_ADD8(cpu, cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x84:  //ADD A, H
                    cpu_instr_ADD8(cpu, cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x85:  //ADD A, L
                    cpu_instr_ADD8(cpu, cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x86:  //ADD A, [HL]
                    cpu_instr_ADD8(cpu, cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x87:  //ADD A, A
                    cpu_instr_ADD8(cpu, cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x88:  //ADC A, B
                    cpu_instr_ADC(cpu, cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x89:  //ADC A, C
                    cpu_instr_ADC(cpu, cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x8A:  //ADC A, D
                    cpu_instr_ADC(cpu, cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x8B:  //ADC A, E
                    cpu_instr_ADC(cpu, cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x8C:  //ADC A, H
                    cpu_instr_ADC(cpu, cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x8D:  //ADC A, L
                    cpu_instr_ADC(cpu, cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x8E:  //ADC A, [HL]
                    cpu_instr_ADC(cpu, cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x8F:  //ADC A, A
                    cpu_instr_ADC(cpu, cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;

        case 0x90: break;
        case 0x91: break;
        case 0x92: break;
        case 0x93: break;
        case 0x94: break;
        case 0x95: break;
        case 0x96: break;
        case 0x97: break;
        case 0x98: break;
        case 0x99: break;
        case 0x9A: break;
        case 0x9B: break;
        case 0x9C: break;
        case 0x9D: break;
        case 0x9E: break;
        case 0x9F: break;

        case 0xA0:  //AND A, B
                    cpu_instr_AND(cpu, cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xA1:  //AND A,  C
                    cpu_instr_AND(cpu, cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xA2:  //AND A, D
                    cpu_instr_AND(cpu, cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xA3:  //AND A, E
                    cpu_instr_AND(cpu, cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xA4:  //AND A, H
                    cpu_instr_AND(cpu, cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xA5:  //AND A, L
                    cpu_instr_AND(cpu, cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xA6:  //AND A, [HL]
                    cpu_instr_AND(cpu, cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0xA7:  //AND A, A
                    cpu_instr_AND(cpu, cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xA8: break;
        case 0xA9: break;
        case 0xAA: break;
        case 0xAB: break;
        case 0xAC: break;
        case 0xAD: break;
        case 0xAE: break;
        case 0xAF: break;

        case 0xB0: break;
        case 0xB1: break;
        case 0xB2: break;
        case 0xB3: break;
        case 0xB4: break;
        case 0xB5: break;
        case 0xB6: break;
        case 0xB7: break;
        case 0xB8:  //CP A, B
                    cpu_instr_CP(cpu, cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xB9:  //CP A, C
                    cpu_instr_CP(cpu, cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xBA:  //CP A, D
                    cpu_instr_CP(cpu, cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xBB:  //CP A, E
                    cpu_instr_CP(cpu, cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xBC:  //CP A, H
                    cpu_instr_CP(cpu, cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xBD:  //CP A, L
                    cpu_instr_CP(cpu, cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xBE:  //CP A, [HL]
                    cpu_instr_CP(cpu, cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0xBF:  //CP A, A
                    cpu_instr_CP(cpu, cpu->AF.r8.hi); //perform 1100 for flag
                    cpu->cycle = 4;
                    break;

        case 0xC0: break;
        case 0xC1: break;
        case 0xC2: break;
        case 0xC3: break;
        case 0xC4: break;
        case 0xC5: break;
        case 0xC6:  //ADD A, n8
                    cpu_instr_ADD8(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xC7: break;
        case 0xC8: break;
        case 0xC9: break;
        case 0xCA: break;
        case 0xCB: break;
        case 0xCC: break;
        case 0xCD: break;
        case 0xCE:  //ADC A, n8
                    cpu_instr_ADC(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xCF: break;

        case 0xD0: break;
        case 0xD1: break;
        case 0xD2: break;
        case 0xD4: break;
        case 0xD5: break;
        case 0xD6: break;
        case 0xD7: break;
        case 0xD8: break;
        case 0xD9: break;
        case 0xDA: break;
        case 0xDC: break;
        case 0xDE: break;
        case 0xDF: break;

        case 0xE0: break;
        case 0xE1: break;
        case 0xE2: break;
        case 0xE5: break;
        case 0xE6:  //AND A, n8
                    cpu_instr_AND(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xE7: break;
        case 0xE8:  //ADD SP, e8
                    cpu_instr_ADDe8(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 16;
                    break;
        case 0xE9: break;
        case 0xEA: break;
        case 0xEE: break;
        case 0xEF: break;

        case 0xF0: break;
        case 0xF1: break;
        case 0xF2: break;
        case 0xF3: break;
        case 0xF5: break;
        case 0xF6: break;
        case 0xF7: break;
        case 0xF8: break;
        case 0xF9: break;
        case 0xFA: break;
        case 0xFB: break;
        case 0xFE:  //CP A, n8
                    cpu_instr_CP(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xFF: break;

        default:    fprintf(stderr, "[ERROR] : ILLEGAL INSTRUCTION");
                    exit(1);
    }
}