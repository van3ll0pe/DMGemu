#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include "cpu_instr.h"

void cpu_init(Cpu* cpu, Memory* memory)
{
    if (!cpu) {
        fprintf(stderr, "[ERROR]: CPU not initialized");
        exit(1);
    }

    if (!memory) { fprintf(stderr, "[ERROR] : BUS NOT LINKED TO THE CPU"); exit(1);}

    cpu->bus = memory;
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

    cpu->IME = false;
    cpu->is_HALT = false;
    cpu->ei_delay = 0;
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

void cpu_updateFlag(Cpu* cpu, Flag flag, bool condition)
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
uint8_t cpu_fetch_byte_pc(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint8_t data = memory_read8(cpu->bus, cpu->PC);
    cpu->PC++;
    return data;
}

//like bus_read16 but read data from PC and increment it +2
uint16_t cpu_fetch_word_pc(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint16_t data = memory_read16(cpu->bus, cpu->PC);
    cpu->PC += 2;
    return data;
}

void cpu_update_ei(Cpu* cpu) {
    if (!cpu) {exit(1);}

    switch  (cpu->ei_delay) {
        case 2 :    cpu->ei_delay = 1;
                    break;
        case 1 :    cpu->ei_delay =  0;
                    cpu->IME = true;
                    break;
        default:    break;
    }
}

void cpu_ticks(Cpu* cpu)
{
    if (!cpu)
        exit(1);

    cpu_update_ei(cpu);
    
    uint32_t ticks = handle_interrupts(cpu);
    switch (ticks) {case 0 : break; default: return ticks;}
        
    if (cpu->is_HALT == true) {
        return 4;
    }

    uint8_t opcode = cpu_fetch_byte_pc(cpu);
    cpu_execute_instruction(cpu, opcode);
}
/********************************   INTERRUPTION MANAGEMENT *******************************************/
static void handle_interrupt(Cpu* cpu, uint8_t interrupt_type, uint16_t interrupt_address)
{
    if (!cpu) {exit(1);}

    //clear the bit requesting interrupt
    uint8_t value = memory_read8(cpu->bus, IF);
    value &= ~interrupt_type;
    memory_write8(cpu->bus, IF, value);

    instr_push(cpu, cpu->PC);
    cpu->PC = interrupt_address;

    cpu->IME = false;
}

uint32_t handle_interrupts(Cpu* cpu)
{
    if (!cpu) { exit(1); }

    uint8_t ie_reg = memory_read8(cpu->bus, IE);
    uint8_t if_reg = memory_read8(cpu->bus, IF);
    uint8_t requested_interrupt = ie_reg & if_reg;

    if (requested_interrupt == 0x0) return 0;

    if (cpu->is_HALT == true) { //halt is desactivated if interrupt is requested even if EMI is false
        cpu->is_HALT = false;

        if (cpu->IME == false) { //HALTBUG
        }
    }

    if (cpu->IME == false)
        return 0;
    
    //VBLANK
    if (VBLANK & requested_interrupt) {
        handle_interrupt(cpu, VBLANK, VBLANK_ADDR);
        return 20;
    }
    //LCD
    if (LCD & requested_interrupt) {
        handle_interrupt(cpu, LCD, LCD_ADDR);
        return 20;
    }
    //TIMER
    if (TIMER & requested_interrupt) {
        handle_interrupt(cpu, TIMER, TIMER_ADDR);
        return 20;
    }
    //SERIAL
    if (SERIAL & requested_interrupt) {
        handle_interrupt(cpu, SERIAL, SERIAL_ADDR);
        return 20;
    }
    //JOYPAD
    if (JOYPAD & requested_interrupt) {
        handle_interrupt(cpu, JOYPAD, JOYPAD_ADDR);
        return 20;
    }

    return 0;
}

/************************************************************************************************************************ */

uint32_t cpu_execute_instruction(Cpu* cpu, uint8_t opcode)
{
    if (!cpu)
        exit(1);
    
    switch (opcode) {
        case 0x00: { return 4; } //NOP
        case 0x01: { cpu->BC.r16 = cpu_fetch_word_pc(cpu); return 12; } //LD BC, n16
        case 0x02: { memory_write8(cpu->bus, cpu->BC.r16, cpu->AF.r8.hi); return 8; } //LD (BC), A
        case 0x03: { instr_inc16(&cpu->BC.r16); return 8; } //INC BC
        case 0x04: return 0;
        case 0x05: return 0;
        case 0x06: { cpu->BC.r8.hi = cpu_fetch_byte_pc(cpu); return 8; } //LD B, n8
        case 0x07: return 0;
        case 0x08: { memory_write16(cpu->bus, cpu_fetch_word_pc(cpu), cpu->SP); return 20; } //LD (a16), SP
        case 0x09: return 0;
        case 0x0A: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->BC.r16); return 8; } //LD A, (BC)
        case 0x0B: { instr_dec16(&cpu->BC.r16); return 8; } //DEC BC
        case 0x0C: return 0;
        case 0x0D: return 0;
        case 0x0E: { cpu->BC.r8.lo = cpu_fetch_byte_pc(cpu); return 8; } //LD C, n8
        case 0x0F: return 0;

        case 0x10: { cpu_fetch_byte_pc(cpu); return 4; } //STOP
        case 0x11: { cpu->DE.r16 = cpu_fetch_word_pc(cpu); return 12; } //LD DE, n16
        case 0x12: { memory_write8(cpu->bus, cpu->DE.r16, cpu->AF.r8.hi); return 8; } //LD (DE), A
        case 0x13: { instr_inc16(&cpu->DE.r16); return 8; } //INC DE
        case 0x14: return 0;
        case 0x15: return 0;
        case 0x16: { cpu->DE.r8.hi = cpu_fetch_byte_pc(cpu); return 8; } //LD D, n8
        case 0x17: return 0;
        case 0x18: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); cpu->PC += e8; return 12; } //JR e8
        case 0x19: return 0;
        case 0x1A: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->DE.r16); return 8; } //LD A, (DE)
        case 0x1B: { instr_dec16(&cpu->DE.r16); return 8; } //DEC DE
        case 0x1C: return 0;
        case 0x1D: return 0;
        case 0x1E: { cpu->DE.r8.lo = cpu_fetch_byte_pc(cpu); return 8; } //LD E, n8
        case 0x1F: return 0;

        case 0x20: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 0) { cpu->PC += e8; return 12; } else { return 8; }} //JR NZ, e8
        case 0x21: { cpu->HL.r16 = cpu_fetch_word_pc(cpu); return 12; } //LD HL, n16
        case 0x22: { memory_write8(cpu->bus, cpu->HL.r16, cpu->AF.r8.hi); cpu->HL.r16++; return 8; } //LD (HL+), A
        case 0x23: { instr_inc16(&cpu->HL.r16); return 8; } //INC HL
        case 0x24: return 0;
        case 0x25: return 0;
        case 0x26: { cpu->HL.r8.hi = cpu_fetch_byte_pc(cpu); return 8; } //LD H, n8
        case 0x27: return 0;
        case 0x28: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 1) { cpu->PC += e8; return 12; } else { return 8; }} //JR Z, e8
        case 0x29: return 0;
        case 0x2A: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->HL.r16); cpu->HL.r16++; return 8; } //LD A, (HL+)
        case 0x2B: { instr_dec16(&cpu->HL.r16); return 8; } //DEC HL
        case 0x2C: return 0;
        case 0x2D: return 0;
        case 0x2E: { cpu->HL.r8.lo = cpu_fetch_byte_pc(cpu); return 8; } //LD L, n8
        case 0x2F: return 0;

        case 0x30: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 0) { cpu->PC += e8; return 12; } else { return 8; }} //JR NC, e8
        case 0x31: { cpu->SP = cpu_fetch_word_pc(cpu); return 12; } //LD SP, n16
        case 0x32: { memory_write8(cpu->bus, cpu->HL.r16, cpu->AF.r8.hi); cpu->HL.r16--; return 8; } //LD (HL-), A
        case 0x33: { instr_inc16(&cpu->SP); return 8; } //INC SP
        case 0x34: return 0;
        case 0x35: return 0;
        case 0x36: { memory_write8(cpu->bus, cpu->HL.r16, cpu_fetch_byte_pc(cpu)); return 12; } //LD (HL), n8
        case 0x37: return 0;
        case 0x38: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 1) { cpu->PC += e8; return 12; } else { return 8; }} //JR C, e8
        case 0x39: return 0;
        case 0x3A: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->HL.r16); cpu->HL.r16--; return 8; } //LD A, (HL-)
        case 0x3B: { instr_dec16(&cpu->SP); return 8; } //DEC SP
        case 0x3C: return 0;
        case 0x3D: return 0;
        case 0x3E: { cpu->AF.r8.hi = cpu_fetch_byte_pc(cpu); return 8; }; //LD A, n8
        case 0x3F: return 0;

        case 0x40:  { cpu->BC.r8.hi = cpu->BC.r8.hi; return 4; } //LD B, B
        case 0x41:  { cpu->BC.r8.hi = cpu->BC.r8.lo; return 4; } //LD B, C
        case 0x42:  { cpu->BC.r8.hi = cpu->DE.r8.hi; return 4; } //LD B, D
        case 0x43:  { cpu->BC.r8.hi = cpu->DE.r8.lo; return 4; } //LD B, E
        case 0x44:  { cpu->BC.r8.hi = cpu->HL.r8.hi; return 4; } //LD B, H
        case 0x45:  { cpu->BC.r8.hi = cpu->HL.r8.lo; return 4; } //LD B, L
        case 0x46:  { cpu->BC.r8.hi = memory_read8(cpu->bus, cpu->HL.r16); return 8; } //LD B, (HL)
        case 0x47:  { cpu->BC.r8.hi = cpu->AF.r8.hi; return 4; } //LD B, A
        case 0x48:  { cpu->BC.r8.lo = cpu->BC.r8.hi; return 4; } //LD C, B
        case 0x49:  { cpu->BC.r8.lo = cpu->BC.r8.lo; return 4; } //LD C, C
        case 0x4A:  { cpu->BC.r8.lo = cpu->DE.r8.hi; return 4; } //LD C, D
        case 0x4B:  { cpu->BC.r8.lo = cpu->DE.r8.lo; return 4; } //LD C, E
        case 0x4C:  { cpu->BC.r8.lo = cpu->HL.r8.hi; return 4; } //LD C, H
        case 0x4D:  { cpu->BC.r8.lo = cpu->HL.r8.lo; return 4; } //LD C, L
        case 0x4E:  { cpu->BC.r8.lo = memory_read8(cpu->bus, cpu->HL.r16); return 8; } //LD C, (HL)
        case 0x4F:  { cpu->BC.r8.lo = cpu->AF.r8.hi; return 4; } //LD C, A

        case 0x50: { cpu->DE.r8.hi = cpu->BC.r8.hi; return 4; } //LD D, B
        case 0x51: { cpu->DE.r8.hi = cpu->BC.r8.lo; return 4; } //LD D, C
        case 0x52: { cpu->DE.r8.hi = cpu->DE.r8.hi; return 4; } //LD D, D
        case 0x53: { cpu->DE.r8.hi = cpu->DE.r8.lo; return 4; } //LD D, E
        case 0x54: { cpu->DE.r8.hi = cpu->HL.r8.hi; return 4; } //LD D, H
        case 0x55: { cpu->DE.r8.hi = cpu->HL.r8.lo; return 4; } //LD D, L
        case 0x56: { cpu->DE.r8.hi = memory_read8(cpu->bus, cpu->HL.r16); return 8; } //LD D, (HL)
        case 0x57: { cpu->DE.r8.hi = cpu->AF.r8.hi; return 4; } //LD D, A
        case 0x58: { cpu->DE.r8.lo = cpu->BC.r8.hi; return 4; } //LD E, B
        case 0x59: { cpu->DE.r8.lo = cpu->BC.r8.lo; return 4; } //LD E, C
        case 0x5A: { cpu->DE.r8.lo = cpu->DE.r8.hi; return 4; } //LD E, D
        case 0x5B: { cpu->DE.r8.lo = cpu->DE.r8.lo; return 4; } //LD E, E
        case 0x5C: { cpu->DE.r8.lo = cpu->HL.r8.hi; return 4; } //LD E, H
        case 0x5D: { cpu->DE.r8.lo = cpu->HL.r8.lo; return 4; } //LD E, L
        case 0x5E: { cpu->DE.r8.lo = memory_read8(cpu->bus, cpu->HL.r16); return 8; } //LD E, (HL)
        case 0x5F: { cpu->DE.r8.lo = cpu->AF.r8.hi; return 4; } //LD E, A

        case 0x60: { cpu->HL.r8.hi = cpu->BC.r8.hi; return 4; } //LD H, B
        case 0x61: { cpu->HL.r8.hi = cpu->BC.r8.lo; return 4; } //LD H, C
        case 0x62: { cpu->HL.r8.hi = cpu->DE.r8.hi; return 4; } //LD H, D
        case 0x63: { cpu->HL.r8.hi = cpu->DE.r8.lo; return 4; } //LD H, E
        case 0x64: { cpu->HL.r8.hi = cpu->HL.r8.hi; return 4; } //LD H, H
        case 0x65: { cpu->HL.r8.hi = cpu->HL.r8.lo; return 4; } //LD H, L
        case 0x66: { cpu->HL.r8.hi = memory_read8(cpu->bus, cpu->HL.r16); return 8; } //LD H, (HL)
        case 0x67: { cpu->HL.r8.hi = cpu->AF.r8.hi; return 4; } //LD H, A
        case 0x68: { cpu->HL.r8.lo = cpu->BC.r8.hi; return 4; } //LD L, B
        case 0x69: { cpu->HL.r8.lo = cpu->BC.r8.lo; return 4; } //LD L, C
        case 0x6A: { cpu->HL.r8.lo = cpu->DE.r8.hi; return 4; } //LD L, D
        case 0x6B: { cpu->HL.r8.lo = cpu->DE.r8.lo; return 4; } //LD L, E
        case 0x6C: { cpu->HL.r8.lo = cpu->HL.r8.hi; return 4; } //LD L, H
        case 0x6D: { cpu->HL.r8.lo = cpu->HL.r8.lo; return 4; } //LD L, L
        case 0x6E: { cpu->HL.r8.lo = memory_read8(cpu->bus, cpu->HL.r16); return 8; } //LD L, (HL)
        case 0x6F: { cpu->HL.r8.lo = cpu->AF.r8.hi; return 4; } //LD L, A

        case 0x70: { memory_write8(cpu->bus, cpu->HL.r16, cpu->BC.r8.hi); return 8; } //LD (HL), B
        case 0x71: { memory_write8(cpu->bus, cpu->HL.r16, cpu->BC.r8.lo); return 8; } //LD (HL), C
        case 0x72: { memory_write8(cpu->bus, cpu->HL.r16, cpu->DE.r8.hi); return 8; } //LD (HL), D
        case 0x73: { memory_write8(cpu->bus, cpu->HL.r16, cpu->DE.r8.lo); return 8; } //LD (HL), E
        case 0x74: { memory_write8(cpu->bus, cpu->HL.r16, cpu->HL.r8.hi); return 8; } //LD (HL), H
        case 0x75: { memory_write8(cpu->bus, cpu->HL.r16, cpu->HL.r8.lo); return 8; } //LD (HL), L
        case 0x76: { cpu->is_HALT = true; return 4; } //HALT
        case 0x77: { memory_write8(cpu->bus, cpu->HL.r16, cpu->AF.r8.hi); return 8; } //LD (HL), A
        case 0x78: { cpu->AF.r8.hi = cpu->BC.r8.hi; return 4; } //LD A, B
        case 0x79: { cpu->AF.r8.hi = cpu->BC.r8.lo; return 4; } //LD A, C
        case 0x7A: { cpu->AF.r8.hi = cpu->DE.r8.hi; return 4; } //LD A, D
        case 0x7B: { cpu->AF.r8.hi = cpu->DE.r8.lo; return 4; } //LD A, E
        case 0x7C: { cpu->AF.r8.hi = cpu->HL.r8.hi; return 4; } //LD A, H
        case 0x7D: { cpu->AF.r8.hi = cpu->HL.r8.lo; return 4; } //LD A, L
        case 0x7E: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->HL.r16); return 8; } //LD A, (HL)
        case 0x7F: { cpu->AF.r8.hi = cpu->AF.r8.hi; return 4; } //LD A, A

        case 0x80: return 0;
        case 0x81: return 0;
        case 0x82: return 0;
        case 0x83: return 0;
        case 0x84: return 0;
        case 0x85: return 0;
        case 0x86: return 0;
        case 0x87: return 0;
        case 0x88: return 0;
        case 0x89: return 0;
        case 0x8A: return 0;
        case 0x8B: return 0;
        case 0x8C: return 0;
        case 0x8D: return 0;
        case 0x8E: return 0;
        case 0x8F: return 0;

        case 0x90: return 0;
        case 0x91: return 0;
        case 0x92: return 0;
        case 0x93: return 0;
        case 0x94: return 0;
        case 0x95: return 0;
        case 0x96: return 0;
        case 0x97: return 0;
        case 0x98: return 0;
        case 0x99: return 0;
        case 0x9A: return 0;
        case 0x9B: return 0;
        case 0x9C: return 0;
        case 0x9D: return 0;
        case 0x9E: return 0;
        case 0x9F: return 0;

        case 0xA0: return 0;
        case 0xA1: return 0;
        case 0xA2: return 0;
        case 0xA3: return 0;
        case 0xA4: return 0;
        case 0xA5: return 0;
        case 0xA6: return 0;
        case 0xA7: return 0;
        case 0xA8: return 0;
        case 0xA9: return 0;
        case 0xAA: return 0;
        case 0xAB: return 0;
        case 0xAC: return 0;
        case 0xAD: return 0;
        case 0xAE: return 0;
        case 0xAF: return 0;

        case 0xB0: return 0;
        case 0xB1: return 0;
        case 0xB2: return 0;
        case 0xB3: return 0;
        case 0xB4: return 0;
        case 0xB5: return 0;
        case 0xB6: return 0;
        case 0xB7: return 0;
        case 0xB8: return 0;
        case 0xB9: return 0;
        case 0xBA: return 0;
        case 0xBB: return 0;
        case 0xBC: return 0;
        case 0xBD: return 0;
        case 0xBE: return 0;
        case 0xBF: return 0;

        case 0xC0: { if (cpu_getFlag(cpu, Z_FLAG) == 0) { cpu->PC = instr_pop(cpu); return 20; } else { return 8; } } //RET NZ
        case 0xC1: { cpu->BC.r16 = instr_pop(cpu); return 12; } //POP BC
        case 0xC2: {uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 0) { cpu->PC = a16; return 16; } else { return 12; }} //JP NZ, a16 (else, pc, is already move to next instr)
        case 0xC3: { cpu->PC = cpu_fetch_word_pc(cpu); return 16; } //JP a16
        case 0xC4: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 0) {instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } else { return 12; }} //CALL NZ, a16
        case 0xC5: { instr_push(cpu, cpu->BC.r16); return 16; } //PUSH BC
        case 0xC6: return 0;
        case 0xC7: { instr_push(cpu, cpu->PC); cpu->PC = 0x0000; return 16; } //RST $00
        case 0xC8: { if (cpu_getFlag(cpu, Z_FLAG) == 1) { cpu->PC = instr_pop(cpu); return 20; } else { return 8; } } //RET Z
        case 0xC9: { cpu->PC = instr_pop(cpu); return 16; } //RET
        case 0xCA: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 1) { cpu->PC = a16; return 16; } else { return 12; }} //JP Z, a16
        case 0xCB: { uint8_t opcb = cpu_fetch_byte_pc(cpu); return cpu_execute_instruction_CB(cpu, opcb) + 4; } //PREFIX CB
        case 0xCC: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 1) {instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } else { return 12; }} //CALL Z, a16
        case 0xCD: { uint16_t a16 = cpu_fetch_word_pc(cpu); instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } //CALL a16
        case 0xCE: return 0;
        case 0xCF: { instr_push(cpu, cpu->PC); cpu->PC = 0x0008; return 16; } //RST $08

        case 0xD0: { if (cpu_getFlag(cpu, C_FLAG) == 0) { cpu->PC = instr_pop(cpu); return 20; } else { return 8; } } //RET NC
        case 0xD1: { cpu->DE.r16 = instr_pop(cpu); return 12; } //POP DE
        case 0xD2: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 0) { cpu->PC = a16; return 16; } else { return 12; }} //JP NC, a16
        case 0xD4: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 0) { instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } else { return 12; }} //CALL NC, a16
        case 0xD5: { instr_push(cpu, cpu->DE.r16); return 16; } //PUSH DE
        case 0xD6: return 0;
        case 0xD7: { instr_push(cpu, cpu->PC); cpu->PC = 0x0010; return 16; } //RST $10
        case 0xD8: { if (cpu_getFlag(cpu, C_FLAG) == 1) { cpu->PC = instr_pop(cpu); return 20; } else { return 8; } } //RET C
        case 0xD9: { cpu->IME = true; cpu->PC = instr_pop(cpu); return 16; } //RETI (no delay time like ei to enable IME)
        case 0xDA: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 1) { cpu->PC = a16; return 16; } else { return 12; }} //JP C, a16
        case 0xDC: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 1) { instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } else { return 12; }} //CALL C, a16
        case 0xDE: return 0;
        case 0xDF: { instr_push(cpu, cpu->PC); cpu->PC = 0x0018; return 16; } //RST $18

        case 0xE0: { memory_write8(cpu->bus, (0xFF00 + cpu_fetch_byte_pc(cpu)), cpu->AF.r8.hi); return 12; } //LDH (a8), A
        case 0xE1: { cpu->HL.r16 = instr_pop(cpu); return 12; } //POP HL
        case 0xE2: { memory_write8(cpu->bus, (0xFF00 + cpu->BC.r8.lo), cpu->AF.r8.hi); return 8; } //LD (C), A
        case 0xE5: { instr_push(cpu, cpu->HL.r16); return 16; } //PUSH HL
        case 0xE6: return 0;
        case 0xE7: { instr_push(cpu, cpu->PC); cpu->PC = 0x0020; return 16; } //RST $20
        case 0xE8: return 0;
        case 0xE9: { cpu->PC = cpu->HL.r16; return 4; } //JP HL
        case 0xEA: { memory_write16(cpu->bus, cpu_fetch_word_pc(cpu), cpu->AF.r8.hi); return 16; } //LD (a16), A
        case 0xEE: return 0;
        case 0xEF: { instr_push(cpu, cpu->PC); cpu->PC = 0x0028; return 16; } //RST $28

        case 0xF0: { cpu->AF.r8.hi = memory_read8(cpu->bus, (0xFF00 + cpu_fetch_byte_pc(cpu))); return 12; } //LDH A, (n8)
        case 0xF1: { cpu->AF.r16 = instr_pop(cpu); return 12; } //POP AF
        case 0xF2: { cpu->AF.r8.hi = memory_read8(cpu->bus, (0xFF00 + cpu->BC.r8.lo)); return 8; } //LD A, (C)
        case 0xF3: { cpu->IME = false; cpu->ei_delay = 0; return 4; } //DI
        case 0xF5: { instr_push(cpu, cpu->AF.r16); return 16; } //PUSH AF
        case 0xF6: return 0;
        case 0xF7: { instr_push(cpu, cpu->PC); cpu->PC = 0x0030; return 16; } //RST 0x30
        case 0xF8: return 0;
        case 0xF9: { cpu->SP = cpu->HL.r16; return 8; } //LD SP, HL
        case 0xFA: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu_fetch_word_pc(cpu)); return 16; } //LD A, (a16)
        case 0xFB: { cpu->ei_delay = 2; return 4; } //EI
        case 0xFE: return 0;
        case 0xFF: { instr_push(cpu, cpu->PC); cpu->PC = 0x0038; return 16; } //RST $38

        default: { fprintf(stderr, "[ERROR] : ILLEGAL INSTRUCTION"); exit(1); }
    }
}

uint32_t cpu_execute_instruction_CB(Cpu* cpu, uint8_t opcode)
{
    if (!cpu)
        exit(1);

    switch (opcode) {
       

        default: fprintf(stderr, "[ERROR] : ILLEGAL INSTRUCTION CB");
                    exit(1);
    }
}