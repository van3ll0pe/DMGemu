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

    cpu->IME = false;
    cpu->HALT = false;
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

void cpu_setFlag(Cpu* cpu, Flag flag)
{
    if (!cpu)
        exit(1);
    
    cpu->AF.r8.lo |= flag;
}

void cpu_clearFlag(Cpu* cpu, Flag flag)
{
    if (!cpu)
        exit(1);
    
    cpu->AF.r8.lo &= (~flag);
}

void cpu_checkFlag(Cpu* cpu, Flag flag, bool condition)
{
    if (!cpu)
        exit(1);
    
    if (condition)
        cpu_setFlag(cpu, flag);
    else
        cpu_clearFlag(cpu, flag);
}

uint8_t cpu_getFlag(Cpu* cpu, Flag flag)
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
        case 0x00:  //NOP
                    cpu->cycle = 4;
                    break;
        case 0x01:  //LD BC, n16
                    cpu_instr_LDr16_16(&(cpu->BC.r16), cpu_getPCImm16(cpu));
                    cpu->cycle = 12;
                    break;
        case 0x02:  //LD [BC], A
                    cpu_instr_LDmem16_8(cpu, cpu->BC.r16, cpu->AF.r8.hi);
                    cpu->cycle = 8;
                    break;
        case 0x03:  //INC BC
                    cpu_instr_INC16(cpu, &cpu->BC.r16);
                    cpu->cycle = 8;
                    break;
        case 0x04:  //INC B
                    cpu_instr_INC8(cpu, &cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x05:  //DEC B
                    cpu_instr_DEC8(cpu, &cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x06:  //LD B, n8
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0x07: break;
        case 0x08:  //LD [a16], SP
                    cpu_instr_LDmem16_16(cpu, cpu_getPCImm16(cpu), cpu->SP);
                    cpu->cycle = 20;
                    break;
        case 0x09:  //ADD HL, BC
                    cpu_instr_ADD16(cpu, cpu->BC.r16);
                    cpu->cycle = 8;
                    break;
        case 0x0A:  //LD A, [BC]
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu->BC.r16));
                    cpu->cycle = 8;
                    break;
        case 0x0B:  //DEC BC
                    cpu_instr_DEC16(cpu, &cpu->BC.r16);
                    cpu->cycle = 8;
                    break;
        case 0x0C:  //INC C
                    cpu_instr_INC8(cpu, &cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x0D:  //DEC C
                    cpu_instr_DEC8(cpu, &cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x0E:  //LD C, n8
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0x0F: break;
        
        case 0x10:  //STOP
                    cpu_getPCImm8(cpu);
                    cpu->cycle = 4;
                    //do nothing
                    break;
        case 0x11:  //LD DE, n16
                    cpu_instr_LDr16_16(&(cpu->DE.r16), cpu_getPCImm16(cpu));
                    cpu->cycle = 12;
                    break;
        case 0x12:  //LD [DE], A
                    cpu_instr_LDmem16_8(cpu, cpu->DE.r16, cpu->AF.r8.hi);
                    cpu->cycle = 8;
                    break;
        case 0x13:  //INC DE
                    cpu_instr_INC16(cpu, &cpu->DE.r16);
                    cpu->cycle = 8;
                    break;
        case 0x14:  //INC D
                    cpu_instr_INC8(cpu, &cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x15:  //DEC D
                    cpu_instr_DEC8(cpu, &cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x16:  //LD D, n8
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0x17: break;
        case 0x18: break;
        case 0x19:  //ADD HL, DE
                    cpu_instr_ADD16(cpu, cpu->DE.r16);
                    cpu->cycle = 8;
                    break;
        case 0x1A:  //LD A, [DE]
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu->DE.r16));
                    cpu->cycle = 8;
                    break;
        case 0x1B:  //DEC DE
                    cpu_instr_DEC16(cpu, &cpu->DE.r16);
                    cpu->cycle = 8;
                    break;
        case 0x1C:  //INC E
                    cpu_instr_INC8(cpu, &cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x1D:  //DEC E
                    cpu_instr_DEC8(cpu, &cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x1E:  //LD E, n8
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0x1F: break;

        case 0x20: break;
        case 0x21:  //LD HL, n16
                    cpu_instr_LDr16_16(&(cpu->HL.r16), cpu_getPCImm16(cpu));
                    cpu->cycle = 12;
                    break;
        case 0x22:  //LD [HL+], A
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16,cpu->AF.r8.hi);
                    cpu->HL.r16++;
                    cpu->cycle = 8;
                    break;
        case 0x23:  //INC HL
                    cpu_instr_INC16(cpu, &cpu->HL.r16);
                    cpu->cycle = 8;
                    break;
        case 0x24:  //INC H
                    cpu_instr_INC8(cpu, &cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x25:  //DEC H
                    cpu_instr_DEC8(cpu, &cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x26:  //LD H, n8
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0x27: break;
        case 0x28: break;
        case 0x29:  //ADD HL, HL
                    cpu_instr_ADD16(cpu, cpu->HL.r16);
                    cpu->cycle = 8;
                    break;
        case 0x2A:  //LD A, [HL+]
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->HL.r16++;
                    cpu->cycle = 8;
                    break;
        case 0x2B:  //DEC HL
                    cpu_instr_DEC16(cpu, &cpu->HL.r16);
                    cpu->cycle = 8;
                    break;
        case 0x2C:  //INC L
                    cpu_instr_INC8(cpu, &cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x2D:  //DEC L
                    cpu_instr_DEC8(cpu, &cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x2E:  //LD L, n8
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0x2F:  //CPL
                    cpu_instr_CPL(cpu);
                    cpu->cycle = 4;
                    break;

        case 0x30: break;
        case 0x31:  //LD SP, n16
                    cpu_instr_LDr16_16(&(cpu->SP), cpu_getPCImm16(cpu));
                    cpu->cycle = 12;
                    break;
        case 0x32:  //LD [HL-], A
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu->AF.r8.hi);
                    cpu->HL.r16--;
                    cpu->cycle = 8;
                    break;
        case 0x33:  //INC SP
                    cpu_instr_INC16(cpu, &cpu->SP);
                    cpu->cycle = 8;
                    break;
        case 0x34:  //INC [HL]
                    cpu_instr_INCHL(cpu, cpu->HL.r16);
                    cpu->cycle = 12;
                    break;
        case 0x35:  //DEC [HL]
                    cpu_instr_DECHL(cpu, cpu->HL.r16);
                    cpu->cycle = 12;
                    break;
        case 0x36:  //LD [HL], n8
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu_getPCImm8(cpu));
                    cpu->cycle = 12;
                    break;
        case 0x37:  //SCF
                    cpu_instr_SCF(cpu);
                    cpu->cycle = 4;
                    break;
        case 0x38: break;
        case 0x39:  //ADD HL, SP
                    cpu_instr_ADD16(cpu, cpu->SP);
                    cpu->cycle = 8;
                    break;
        case 0x3A:  //LD A, [HL-]
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->HL.r16--;
                    cpu->cycle = 8;
                    break;
        case 0x3B:  //DEC SP
                    cpu_instr_DEC16(cpu, &cpu->SP);
                    cpu->cycle = 8;
                    break;
        case 0x3C:  //INC A
                    cpu_instr_INC8(cpu, &cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x3D:  //DEC A
                    cpu_instr_DEC8(cpu, &cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x3E:  //LD A, n8
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0x3F:  //CCF
                    cpu_instr_CCF(cpu);
                    cpu->cycle = 4;
                    break;

        case 0x40:  //LD B, B
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x41:  //LD B, C
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x42:  //LD B, D
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x43:  //LD B, E
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x44:  //LD B, H
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x45:  //LD B, L
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x46:  //LD B, [HL]
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x47:  //LD B, A
                    cpu_instr_LDr8_8(&(cpu->BC.r8.hi), cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x48:  //LD C, B
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x49:  //LD C, C
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x4A:  //LD C, D
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x4B:  //LD C, E
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x4C:  //LD C, H
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x4D:  //LD C, L
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x4E:  //LD C, [HL]
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x4F:  //LD C, A
                    cpu_instr_LDr8_8(&(cpu->BC.r8.lo), cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;

        case 0x50:  //LD D, B
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x51:  //LD D, C
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x52:  //LD D, D
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x53:  //LD D, E
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x54:  //LD D, H
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x55:  //LD D, L
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x56:  //LD D, [HL]
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x57:  //LD D, A
                    cpu_instr_LDr8_8(&(cpu->DE.r8.hi), cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x58:  //LD E, B
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x59:  //LD E, C
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x5A:  //LD E, D
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x5B:  //LD E, E
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x5C:  //LD E, H
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x5D:  //LD E, L
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x5E:  //LD E, [HL]
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x5F:  //LD E, A
                    cpu_instr_LDr8_8(&(cpu->DE.r8.lo), cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;

        case 0x60:  //LD H, B
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x61:  //LD H, C
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x62:  //LD H, D
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x63:  //LD H, E
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x64:  //LD H, H
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x65:  //LD H, L
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x66:  //LD H, [HL]
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x67:  //LD H, A
                    cpu_instr_LDr8_8(&(cpu->HL.r8.hi), cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x68:  ///LD L, B
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu->BC.r8.hi);
                    cpu->cycle  = 4;
                    break;
        case 0x69:  //LD L, C
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu->BC.r8.lo);
                    cpu->cycle  = 4;
                    break;
        case 0x6A:  //LD L, D
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu->DE.r8.hi);
                    cpu->cycle  = 4;
                    break;
        case 0x6B:  //LD L, E
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu->DE.r8.lo);
                    cpu->cycle  = 4;
                    break;
        case 0x6C:  //LD L, H
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu->HL.r8.hi);
                    cpu->cycle  = 4;
                    break;
        case 0x6D:  //LD L, L
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu->HL.r8.lo);
                    cpu->cycle  = 4;
                    break;
        case 0x6E:  //LD L, [HL]
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle  = 8;
                    break;
        case 0x6F:  //LD L, A
                    cpu_instr_LDr8_8(&(cpu->HL.r8.lo), cpu->AF.r8.hi);
                    cpu->cycle  = 4;
                    break;

        case 0x70:  //LD [HL], B
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu->BC.r8.hi);
                    cpu->cycle = 8;
                    break;
        case 0x71:  //LD [HL], C
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu->BC.r8.lo);
                    cpu->cycle = 8;
                    break;
        case 0x72:  //LD [HL], D
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu->DE.r8.hi);
                    cpu->cycle = 8;
                    break;
        case 0x73:  //LD [HL], E
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu->DE.r8.lo);
                    cpu->cycle = 8;
                    break;
        case 0x74:  //LD [HL], H
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu->HL.r8.hi);
                    cpu->cycle = 8;
                    break;
        case 0x75:  //LD [HL], L
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu->HL.r8.lo);
                    cpu->cycle = 8;
                    break;
        case 0x76: break;
        case 0x77:  ///LD [HL], A
                    cpu_instr_LDmem16_8(cpu, cpu->HL.r16, cpu->AF.r8.hi);
                    cpu->cycle = 8;
                    break;
        case 0x78:  //LD A, B
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x79:  //LD A, C
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x7A:  //LD A, D
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x7B:  //LD A, E
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x7C:  //LD A, H
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x7D:  //LD A, L
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x7E:  //LD A, [HL]
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x7F:  //LD A, A
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;

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

        case 0x90:  //SUB A, B
                    cpu_instr_SUB(cpu, cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x91:  //SUB A, C
                    cpu_instr_SUB(cpu, cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x92:  //SUB A, D
                    cpu_instr_SUB(cpu, cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x93:  //SUB A, E
                    cpu_instr_SUB(cpu, cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x94:  //SUB A, H
                    cpu_instr_SUB(cpu, cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x95:  //SUB A, L
                    cpu_instr_SUB(cpu, cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x96:  //SUB A, [HL]
                    cpu_instr_SUB(cpu, cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x97:  //SUB A, A
                    cpu_instr_SUB(cpu, cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x98:  //SBC A, B
                    cpu_instr_SBC(cpu, cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x99:  //SBC A, C
                    cpu_instr_SBC(cpu, cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x9A:  //SBC A, D
                    cpu_instr_SBC(cpu, cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x9B:  //SBC A, E
                    cpu_instr_SBC(cpu, cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x9C:  //SBC A, H
                    cpu_instr_SBC(cpu, cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0x9D:  //SBC A, L
                    cpu_instr_SBC(cpu, cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0x9E:  //SBC A, [HL]
                    cpu_instr_SBC(cpu, cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0x9F:  //SBC A, A
                    cpu_instr_SBC(cpu, cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;

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
        case 0xA8:  //XOR A, B
                    cpu_instr_XOR(cpu, cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xA9:  //XOR A, C
                    cpu_instr_XOR(cpu, cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xAA:  //XOR A, D
                    cpu_instr_XOR(cpu,  cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xAB:  //XOR A, E
                    cpu_instr_XOR(cpu, cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xAC:  //XOR A, H
                    cpu_instr_XOR(cpu, cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xAD:  //XOR A, L
                    cpu_instr_XOR(cpu, cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xAE:  //XOR A, [HL]
                    cpu_instr_XOR(cpu, cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0xAF:  //XOR A, A
                    cpu_instr_XOR(cpu, cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;

        case 0xB0:  //OR A, B
                    cpu_instr_OR(cpu, cpu->BC.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xB1:  //OR A, C
                    cpu_instr_OR(cpu, cpu->BC.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xB2:  //OR A, D
                    cpu_instr_OR(cpu, cpu->DE.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xB3:  //OR A, E
                    cpu_instr_OR(cpu, cpu->DE.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xB4:  //OR A, H
                    cpu_instr_OR(cpu, cpu->HL.r8.hi);
                    cpu->cycle = 4;
                    break;
        case 0xB5:  //OR A, L
                    cpu_instr_OR(cpu, cpu->HL.r8.lo);
                    cpu->cycle = 4;
                    break;
        case 0xB6:  //OR A, [HL]
                    cpu_instr_OR(cpu, cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16));
                    cpu->cycle = 8;
                    break;
        case 0xB7:  //OR A, A
                    cpu_instr_OR(cpu, cpu->AF.r8.hi);
                    cpu->cycle = 4;
                    break;
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

        case 0xC0:  //RET NZ
                    cpu->cycle = cpu_instr_RET(cpu, (cpu_getFlag(cpu, Z_FLAG) == 0));
                    break;
        case 0xC1:  //POP BC
                    cpu_instr_POP(cpu, &(cpu->BC.r16));
                    cpu->cycle = 12;
                    break;
        case 0xC2:  //JP NZ, a16
                    cpu->cycle = cpu_instr_JP(cpu, (cpu_getFlag(cpu, Z_FLAG) == 0), cpu_getPCImm16(cpu));
                    break;
        case 0xC3:  //JP a16
                    cpu->cycle = cpu_instr_JP(cpu, true, cpu_getPCImm16(cpu)); //condition is true to take the jump
                    break;

        case 0xC4:  //CALL NZ, a16
                    cpu->cycle = cpu_instr_CALL(cpu, (cpu_getFlag(cpu, Z_FLAG) == 0), cpu_getPCImm16(cpu));
                    break;
        case 0xC5:  //PUSH BC
                    cpu_instr_PUSH(cpu, cpu->BC.r16);
                    cpu->cycle = 16;
                    break;
        case 0xC6:  //ADD A, n8
                    cpu_instr_ADD8(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xC7: break;
        case 0xC8:  //RET Z
                    cpu->cycle = cpu_instr_RET(cpu, (cpu_getFlag(cpu, Z_FLAG) == 1));
                    break;
        case 0xC9:  //RET
                    cpu_instr_RET(cpu, true);
                    cpu->cycle = 16;
                    break;
        case 0xCA:  //JP Z, a16
                    cpu->cycle = cpu_instr_JP(cpu, (cpu_getFlag(cpu, Z_FLAG) == 1), cpu_getPCImm16(cpu));
                    break;
        case 0xCB: break;
        case 0xCC:  //CALL Z, a16
                    cpu->cycle = cpu_instr_CALL(cpu, (cpu_getFlag(cpu, Z_FLAG) == 1), cpu_getPCImm16(cpu));
                    break;
        case 0xCD:  //CALL a16
                    cpu->cycle = cpu_instr_CALL(cpu, true, cpu_getPCImm16(cpu));
                    break;
        case 0xCE:  //ADC A, n8
                    cpu_instr_ADC(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xCF: break;

        case 0xD0:  //RET NC
                    cpu->cycle = cpu_instr_RET(cpu, (cpu_getFlag(cpu, C_FLAG) == 0));
                    break;
        case 0xD1:  //POP DE
                    cpu_instr_POP(cpu, &(cpu->DE.r16));
                    cpu->cycle = 12;
                    break;
        case 0xD2:  //JP NC, a16
                    cpu->cycle = cpu_instr_JP(cpu, (cpu_getFlag(cpu, C_FLAG) == 0), cpu_getPCImm16(cpu));
                    break;
        case 0xD4:  //CALL NC, a16
                    cpu->cycle = cpu_instr_CALL(cpu, (cpu_getFlag(cpu, C_FLAG) == 0), cpu_getPCImm16(cpu));
                    break;
        case 0xD5:  //PUSH DE
                    cpu_instr_PUSH(cpu, cpu->DE.r16);
                    cpu->cycle = 16;
                    break;
        case 0xD6:  //SUB A, n8
                    cpu_instr_SUB(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xD7: break;
        case 0xD8:  //RET C
                    cpu->cycle = cpu_instr_RET(cpu, (cpu_getFlag(cpu, C_FLAG) == 1));
                    break;
        case 0xD9:  //RETI
                    cpu_instr_RET(cpu, (true));
                    cpu->IME = true;
                    cpu->cycle = 16;
                    break;
        case 0xDA:  //JP C, a16
                    cpu->cycle = cpu_instr_JP(cpu, (cpu_getFlag(cpu, C_FLAG) == 1), cpu_getPCImm16(cpu));
                    break;
        case 0xDC:  //CALL C, a16
                    cpu->cycle = cpu_instr_CALL(cpu, (cpu_getFlag(cpu, C_FLAG) == 1), cpu_getPCImm16(cpu));
                    break;
        case 0xDE:  //SBC A, n8
                    cpu_instr_SBC(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xDF: break;

        case 0xE0:  //LDH [a8], A
                    cpu_instr_LDmem16_8(cpu, (uint16_t)(0xff00 + cpu_getPCImm8(cpu)), cpu->AF.r8.hi);
                    cpu->cycle = 12;
                    break;
        case 0xE1:  //POP HL
                    cpu_instr_POP(cpu, &(cpu->HL.r16));
                    cpu->cycle = 12;
                    break;
        case 0xE2:  //LD [C], A
                    cpu_instr_LDmem16_8(cpu, (uint16_t)(0xff00 + cpu->BC.r8.lo), cpu->AF.r8.hi);
                    cpu->cycle = 8;
                    break;
        case 0xE5:  //PUSH HL
                    cpu_instr_PUSH(cpu, cpu->HL.r16);
                    cpu->cycle = 16;
                    break;
        case 0xE6:  //AND A, n8
                    cpu_instr_AND(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xE7: break;
        case 0xE8:  //ADD SP, e8
                    cpu_instr_ADDe8(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 16;
                    break;
        case 0xE9:  //JP HL
                    cpu_instr_JP(cpu, true, cpu->HL.r16);
                    cpu->cycle = 4;
                    break;
        case 0xEA:  //LD [a16], A
                    cpu_instr_LDmem16_8(cpu, cpu_getPCImm16(cpu), cpu->AF.r8.hi);
                    cpu->cycle = 16;
                    break;
        case 0xEE:  //XOR A, n8
                    cpu_instr_XOR(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xEF: break;

        case 0xF0:  //LDH A, [a8]
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->bus->bus_read8(cpu->bus->component, (uint16_t)(0xff00 + cpu_getPCImm8(cpu))));
                    cpu->cycle = 12;
                    break;
        case 0xF1:  //POP AF
                    cpu_instr_POP(cpu, &(cpu->AF.r16));
                    cpu->cycle = 12;
                    //no need to manage flags because when pop, F is set with the new bit for flags.
                    break;
        case 0xF2:  //LD A, [C]
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->bus->bus_read8(cpu->bus->component, (uint16_t)(0xff00 + cpu->BC.r8.lo)));
                    cpu->cycle = 8;
                    break;
        case 0xF3:  //DI
                    cpu_instr_DI(cpu);
                    cpu->cycle = 4;
                    break;
        case 0xF5:  //PUSH AF
                    cpu_instr_PUSH(cpu, cpu->AF.r16);
                    cpu->cycle = 16;
                    break;
        case 0xF6:  //OR A, n8
                    cpu_instr_OR(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xF7: break;
        case 0xF8:  //LD HL, SP+e8
                    cpu_instr_LDHLSPe8(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 12;
                    break;
        case 0xF9:  ///LD SP, HL
                    cpu_instr_LDr16_16(&(cpu->SP), cpu->HL.r16);
                    cpu->cycle = 8;
                    break;
        case 0xFA:  //LD A, [a16]
                    cpu_instr_LDr8_8(&(cpu->AF.r8.hi), cpu->bus->bus_read8(cpu->bus->component, cpu_getPCImm16(cpu)));
                    cpu->cycle = 16;
                    break;
        case 0xFB:  //EI
                    cpu_instr_EI(cpu);
                    cpu->cycle = 4;
                    break;
        case 0xFE:  //CP A, n8
                    cpu_instr_CP(cpu, cpu_getPCImm8(cpu));
                    cpu->cycle = 8;
                    break;
        case 0xFF: break;

        default:    fprintf(stderr, "[ERROR] : ILLEGAL INSTRUCTION");
                    exit(1);
    }
}