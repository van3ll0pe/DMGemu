#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include "cpu_instr.h"

void cpu_init(Cpu* cpu, Memory* memory)
{
    if (!cpu) {
        fprintf(stderr, "[ERROR]: CPU not initialized");
        abort();
    }

    if (!memory) { fprintf(stderr, "[ERROR] : BUS NOT LINKED TO THE CPU"); abort();}

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
        abort();
    
    cpu->AF.r8.lo |= flag;
}

void cpu_clearFlag(Cpu* cpu, Flag flag)
{
    if (!cpu)
        abort();
    
    cpu->AF.r8.lo &= (~flag);
}

void cpu_updateFlag(Cpu* cpu, Flag flag, bool condition)
{
    if (!cpu)
        abort();
    
    if (condition)
        cpu_setFlag(cpu, flag);
    else
        cpu_clearFlag(cpu, flag);
}

uint8_t cpu_getFlag(Cpu* cpu, Flag flag)
{
    if (!cpu)
        abort();
    
    return (cpu->AF.r8.lo & flag) ? 1 : 0;
}

//like bus_read8 but read data from PC and increment it
uint8_t cpu_fetch_byte_pc(Cpu* cpu)
{
    if (!cpu)
        abort();
    
    uint8_t data = memory_read8(cpu->bus, cpu->PC);
    cpu->PC++;
    return data;
}

//like bus_read16 but read data from PC and increment it +2
uint16_t cpu_fetch_word_pc(Cpu* cpu)
{
    if (!cpu)
        abort();
    
    uint16_t data = memory_read16(cpu->bus, cpu->PC);
    cpu->PC += 2;
    return data;
}

void cpu_update_ei(Cpu* cpu) {
    if (!cpu) {abort();}

    switch  (cpu->ei_delay) {
        case 2 :    cpu->ei_delay = 1;
                    break;
        case 1 :    cpu->ei_delay =  0;
                    cpu->IME = true;
                    break;
        default:    break;
    }
}

uint32_t cpu_ticks(Cpu* cpu)
{
    if (!cpu)
        abort();

    cpu_update_ei(cpu);
    
    uint32_t ticks = handle_interrupts(cpu);
    switch (ticks) {case 0 : break; default: return ticks;}
        
    if (cpu->is_HALT == true) {
        return 4;
    }

    uint8_t opcode = cpu_fetch_byte_pc(cpu);
    return cpu_execute_instruction(cpu, opcode);
}
/********************************   INTERRUPTION MANAGEMENT *******************************************/
static void handle_interrupt(Cpu* cpu, uint8_t interrupt_type, uint16_t interrupt_address)
{
    if (!cpu) {abort();}

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
    if (!cpu) { abort(); }

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
        abort();

    #ifdef DEBUG
        printf("OPCODE = %2X\n", opcode);
    #endif
    
    switch (opcode) {
        case 0x00: { return 4; } //NOP
        case 0x01: { cpu->BC.r16 = cpu_fetch_word_pc(cpu); return 12; } //LD BC, n16
        case 0x02: { memory_write8(cpu->bus, cpu->BC.r16, cpu->AF.r8.hi); return 8; } //LD (BC), A
        case 0x03: { instr_inc16(&cpu->BC.r16); return 8; } //INC BC
        case 0x04: { cpu->BC.r8.hi = instr_inc8(cpu, cpu->BC.r8.hi); return 4; } //INC B
        case 0x05: { cpu->BC.r8.hi = instr_dec8(cpu, cpu->BC.r8.hi); return 4; } //DEC B
        case 0x06: { cpu->BC.r8.hi = cpu_fetch_byte_pc(cpu); return 8; } //LD B, n8
        case 0x07: { cpu->AF.r8.hi = instr_rlc(cpu, cpu->AF.r8.hi); cpu_updateFlag(cpu, Z_FLAG, false); return 4; } //RLCA
        case 0x08: { memory_write16(cpu->bus, cpu_fetch_word_pc(cpu), cpu->SP); return 20; } //LD (a16), SP
        case 0x09: { instr_add16(cpu, cpu->BC.r16); return 8; } //ADD HL, BC
        case 0x0A: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->BC.r16); return 8; } //LD A, (BC)
        case 0x0B: { instr_dec16(&cpu->BC.r16); return 8; } //DEC BC
        case 0x0C: { cpu->BC.r8.lo = instr_inc8(cpu, cpu->BC.r8.lo); return 4; } //INC C
        case 0x0D: { cpu->BC.r8.lo = instr_dec8(cpu, cpu->BC.r8.lo); return 4; } //DEC C
        case 0x0E: { cpu->BC.r8.lo = cpu_fetch_byte_pc(cpu); return 8; } //LD C, n8
        case 0x0F: {cpu->AF.r8.hi = instr_rrc(cpu, cpu->AF.r8.hi); cpu_updateFlag(cpu, Z_FLAG, false); return 4; } //RRCA

        case 0x10: { cpu_fetch_byte_pc(cpu); return 4; } //STOP
        case 0x11: { cpu->DE.r16 = cpu_fetch_word_pc(cpu); return 12; } //LD DE, n16
        case 0x12: { memory_write8(cpu->bus, cpu->DE.r16, cpu->AF.r8.hi); return 8; } //LD (DE), A
        case 0x13: { instr_inc16(&cpu->DE.r16); return 8; } //INC DE
        case 0x14: { cpu->DE.r8.hi = instr_inc8(cpu, cpu->DE.r8.hi); return 4; } //INC D
        case 0x15: { cpu->DE.r8.hi = instr_dec8(cpu, cpu->DE.r8.hi); return 4; } //DEC D
        case 0x16: { cpu->DE.r8.hi = cpu_fetch_byte_pc(cpu); return 8; } //LD D, n8
        case 0x17: { cpu->AF.r8.hi = instr_rl(cpu, cpu->AF.r8.hi); cpu_updateFlag(cpu, Z_FLAG, false); return 4; } //RLA
        case 0x18: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); instr_jr(cpu, e8); return 12; } //JR e8
        case 0x19: { instr_add16(cpu, cpu->DE.r16); return 8; } //ADD HL, DE
        case 0x1A: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->DE.r16); return 8; } //LD A, (DE)
        case 0x1B: { instr_dec16(&cpu->DE.r16); return 8; } //DEC DE
        case 0x1C: { cpu->DE.r8.lo = instr_inc8(cpu, cpu->DE.r8.lo); return 4; } //INC E
        case 0x1D: { cpu->DE.r8.lo = instr_dec8(cpu, cpu->DE.r8.lo); return 4; } //DEC E
        case 0x1E: { cpu->DE.r8.lo = cpu_fetch_byte_pc(cpu); return 8; } //LD E, n8
        case 0x1F: { cpu->AF.r8.hi = instr_rr(cpu, cpu->AF.r8.hi); cpu_updateFlag(cpu, Z_FLAG, false); return 4; } //RRA

        case 0x20: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 0) { instr_jr(cpu, e8); return 12; } else { return 8; }} //JR NZ, e8
        case 0x21: { cpu->HL.r16 = cpu_fetch_word_pc(cpu); return 12; } //LD HL, n16
        case 0x22: { memory_write8(cpu->bus, cpu->HL.r16, cpu->AF.r8.hi); cpu->HL.r16++; return 8; } //LD (HL+), A
        case 0x23: { instr_inc16(&cpu->HL.r16); return 8; } //INC HL
        case 0x24: { cpu->HL.r8.hi = instr_inc8(cpu, cpu->HL.r8.hi); return 4; } //INC H
        case 0x25: { cpu->HL.r8.hi = instr_dec8(cpu, cpu->HL.r8.hi); return 4; } //DEC H
        case 0x26: { cpu->HL.r8.hi = cpu_fetch_byte_pc(cpu); return 8; } //LD H, n8
        case 0x27: { instr_daa(cpu); return 4; } //DAA
        case 0x28: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 1) { instr_jr(cpu, e8); return 12; } else { return 8; }} //JR Z, e8

        case 0x29: { instr_add16(cpu, cpu->HL.r16); return 8; } //ADD HL, HL
        case 0x2A: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->HL.r16); cpu->HL.r16++; return 8; } //LD A, (HL+)
        case 0x2B: { instr_dec16(&cpu->HL.r16); return 8; } //DEC HL
        case 0x2C: { cpu->HL.r8.lo = instr_inc8(cpu, cpu->HL.r8.lo); return 4; } //INC L
        case 0x2D: { cpu->HL.r8.lo = instr_dec8(cpu, cpu->HL.r8.lo); return 4; } //DEC L
        case 0x2E: { cpu->HL.r8.lo = cpu_fetch_byte_pc(cpu); return 8; } //LD L, n8
        case 0x2F: { instr_cpl(cpu); return 4; } //CPL

        case 0x30: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 0) { instr_jr(cpu, e8); return 12; } else { return 8; }} //JR NC, e8
        case 0x31: { cpu->SP = cpu_fetch_word_pc(cpu); return 12; } //LD SP, n16
        case 0x32: { memory_write8(cpu->bus, cpu->HL.r16, cpu->AF.r8.hi); cpu->HL.r16--; return 8; } //LD (HL-), A
        case 0x33: { instr_inc16(&cpu->SP); return 8; } //INC SP
        case 0x34: { memory_write8(cpu->bus, cpu->HL.r16, instr_inc8(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 12; } //INC (HL)
        case 0x35: { memory_write8(cpu->bus, cpu->HL.r16, instr_dec8(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 12; } //DEC (HL)
        case 0x36: { memory_write8(cpu->bus, cpu->HL.r16, cpu_fetch_byte_pc(cpu)); return 12; } //LD (HL), n8
        case 0x37: { instr_scf(cpu); return 4; } //SCF
        case 0x38: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 1) { instr_jr(cpu, e8); return 12; } else { return 8; }} //JR C, e8
        case 0x39: { instr_add16(cpu, cpu->SP); return 8; } //ADD HL, SP
        case 0x3A: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu->HL.r16); cpu->HL.r16--; return 8; } //LD A, (HL-)
        case 0x3B: { instr_dec16(&cpu->SP); return 8; } //DEC SP
        case 0x3C: { cpu->AF.r8.hi = instr_inc8(cpu, cpu->AF.r8.hi); return 4; } //INC A
        case 0x3D: { cpu->AF.r8.hi = instr_dec8(cpu, cpu->AF.r8.hi); return 4; } //DEC A
        case 0x3E: { cpu->AF.r8.hi = cpu_fetch_byte_pc(cpu); return 8; }; //LD A, n8
        case 0x3F: { instr_ccf(cpu); return 4; } //CCF

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

        case 0x80: { instr_add8(cpu, cpu->BC.r8.hi); return 4; } //ADD A, B
        case 0x81: { instr_add8(cpu, cpu->BC.r8.lo); return 4; } //ADD A, C
        case 0x82: { instr_add8(cpu, cpu->DE.r8.hi); return 4; } //ADD A, D
        case 0x83: { instr_add8(cpu, cpu->DE.r8.lo); return 4; } //ADD A, E
        case 0x84: { instr_add8(cpu, cpu->HL.r8.hi); return 4; } //ADD A, H
        case 0x85: { instr_add8(cpu, cpu->HL.r8.lo); return 4; } //ADD A, L
        case 0x86: { instr_add8(cpu, memory_read8(cpu->bus, cpu->HL.r16)); return 8; } //ADD A, (HL)
        case 0x87: { instr_add8(cpu, cpu->AF.r8.hi); return 4; } //ADD A, A
        case 0x88: { instr_adc(cpu, cpu->BC.r8.hi); return 4; } //ADC A, B
        case 0x89: { instr_adc(cpu, cpu->BC.r8.lo); return 4; } //ADC A, C
        case 0x8A: { instr_adc(cpu, cpu->DE.r8.hi); return 4; } //ADC A, D
        case 0x8B: { instr_adc(cpu, cpu->DE.r8.lo); return 4; } //ADC A, E
        case 0x8C: { instr_adc(cpu, cpu->HL.r8.hi); return 4; } //ADC A, H
        case 0x8D: { instr_adc(cpu, cpu->HL.r8.lo); return 4; } //ADC A, L
        case 0x8E: { instr_adc(cpu, memory_read8(cpu->bus, cpu->HL.r16)); return 8; } //ADC A, (HL)
        case 0x8F: { instr_adc(cpu, cpu->AF.r8.hi); return 4; } //ADC A, A

        case 0x90: { instr_sub(cpu, cpu->BC.r8.hi); return 4; } //SUB A, B
        case 0x91: { instr_sub(cpu, cpu->BC.r8.lo); return 4; } //SUB A, C
        case 0x92: { instr_sub(cpu, cpu->DE.r8.hi); return 4; } //SUB A, D
        case 0x93: { instr_sub(cpu, cpu->DE.r8.lo); return 4; } //SUB A, E
        case 0x94: { instr_sub(cpu, cpu->HL.r8.hi); return 4; } //SUB A, H
        case 0x95: { instr_sub(cpu, cpu->HL.r8.lo); return 4; } //SUB A, L
        case 0x96: { instr_sub(cpu, memory_read8(cpu->bus, cpu->HL.r16)); return 8; } //SUB A, (HL)
        case 0x97: { instr_sub(cpu, cpu->AF.r8.hi); return 4; } //SUB A, A
        case 0x98: { instr_sbc(cpu, cpu->BC.r8.hi); return 4; } //SBC A, B
        case 0x99: { instr_sbc(cpu, cpu->BC.r8.lo); return 4; } //SBC A, C
        case 0x9A: { instr_sbc(cpu, cpu->DE.r8.hi); return 4; } //SBC A, D
        case 0x9B: { instr_sbc(cpu, cpu->DE.r8.lo); return 4; } //SBC A, E
        case 0x9C: { instr_sbc(cpu, cpu->HL.r8.hi); return 4; } //SBC A, H
        case 0x9D: { instr_sbc(cpu, cpu->HL.r8.lo); return 4; } //SBC A, L
        case 0x9E: { instr_sbc(cpu, memory_read8(cpu->bus, cpu->HL.r16)); return 8; } //SBC A, (HL)
        case 0x9F: { instr_sbc(cpu, cpu->AF.r8.hi); return 4; } //SBC A, A

        case 0xA0: { instr_and(cpu, cpu->BC.r8.hi); return 4; } //AND A, B
        case 0xA1: { instr_and(cpu, cpu->BC.r8.lo); return 4; } //AND A, C
        case 0xA2: { instr_and(cpu, cpu->DE.r8.hi); return 4; } //AND A, D
        case 0xA3: { instr_and(cpu, cpu->DE.r8.lo); return 4; } //AND A, E
        case 0xA4: { instr_and(cpu, cpu->HL.r8.hi); return 4; } //AND A, H
        case 0xA5: { instr_and(cpu, cpu->HL.r8.lo); return 4; } //AND A, L
        case 0xA6: { instr_and(cpu, memory_read8(cpu->bus, cpu->HL.r16)); return 8; } //AND A, (HL)
        case 0xA7: { instr_and(cpu, cpu->AF.r8.hi); return 4; } //AND A, A
        case 0xA8: { instr_xor(cpu, cpu->BC.r8.hi); return 4; } //XOR A, B
        case 0xA9: { instr_xor(cpu, cpu->BC.r8.lo); return 4; } //XOR A, C
        case 0xAA: { instr_xor(cpu, cpu->DE.r8.hi); return 4; } //XOR A, D
        case 0xAB: { instr_xor(cpu, cpu->DE.r8.lo); return 4; } //XOR A, E
        case 0xAC: { instr_xor(cpu, cpu->HL.r8.hi); return 4; } //XOR A, H
        case 0xAD: { instr_xor(cpu, cpu->HL.r8.lo); return 4; } //XOR A, L
        case 0xAE: { instr_xor(cpu, memory_read8(cpu->bus, cpu->HL.r16)); return 8; } //XOR A, (HL)
        case 0xAF: { instr_xor(cpu, cpu->AF.r8.hi); return 4; } //XOR A, A

        case 0xB0: { instr_or(cpu, cpu->BC.r8.hi); return 4; } //OR A, B
        case 0xB1: { instr_or(cpu, cpu->BC.r8.lo); return 4; } //OR A, C
        case 0xB2: { instr_or(cpu, cpu->DE.r8.hi); return 4; } //OR A, D
        case 0xB3: { instr_or(cpu, cpu->DE.r8.lo); return 4; } //OR A, E
        case 0xB4: { instr_or(cpu, cpu->HL.r8.hi); return 4; } //OR A, H
        case 0xB5: { instr_or(cpu, cpu->HL.r8.lo); return 4; } //OR A, L
        case 0xB6: { instr_or(cpu, memory_read8(cpu->bus, cpu->HL.r16)); return 8; } //OR A, (HL)
        case 0xB7: { instr_or(cpu, cpu->AF.r8.hi); return 4; } //OR A, A
        case 0xB8: { instr_cp(cpu, cpu->BC.r8.hi); return 4; } //CP A, B
        case 0xB9: { instr_cp(cpu, cpu->BC.r8.lo); return 4; } //CP A, C
        case 0xBA: { instr_cp(cpu, cpu->DE.r8.hi); return 4; } //CP A, D
        case 0xBB: { instr_cp(cpu, cpu->DE.r8.lo); return 4; } //CP A, E
        case 0xBC: { instr_cp(cpu, cpu->HL.r8.hi); return 4; } //CP A, H
        case 0xBD: { instr_cp(cpu, cpu->HL.r8.lo); return 4; } //CP A, L
        case 0xBE: { instr_cp(cpu, memory_read8(cpu->bus, cpu->HL.r16)); return 8; } //CP A, (HL)
        case 0xBF: { instr_cp(cpu, cpu->AF.r8.hi); return 4; } //CP A, A

        case 0xC0: { if (cpu_getFlag(cpu, Z_FLAG) == 0) { cpu->PC = instr_pop(cpu); return 20; } else { return 8; } } //RET NZ
        case 0xC1: { cpu->BC.r16 = instr_pop(cpu); return 12; } //POP BC
        case 0xC2: {uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 0) { cpu->PC = a16; return 16; } else { return 12; }} //JP NZ, a16 (else, pc, is already move to next instr)
        case 0xC3: { cpu->PC = cpu_fetch_word_pc(cpu); return 16; } //JP a16
        case 0xC4: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 0) {instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } else { return 12; }} //CALL NZ, a16
        case 0xC5: { instr_push(cpu, cpu->BC.r16); return 16; } //PUSH BC
        case 0xC6: { instr_add8(cpu, cpu_fetch_byte_pc(cpu)); return 8; } //ADD A, n8
        case 0xC7: { instr_push(cpu, cpu->PC); cpu->PC = 0x0000; return 16; } //RST $00
        case 0xC8: { if (cpu_getFlag(cpu, Z_FLAG) == 1) { cpu->PC = instr_pop(cpu); return 20; } else { return 8; } } //RET Z
        case 0xC9: { cpu->PC = instr_pop(cpu); return 16; } //RET
        case 0xCA: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 1) { cpu->PC = a16; return 16; } else { return 12; }} //JP Z, a16
        case 0xCB: { uint8_t opcb = cpu_fetch_byte_pc(cpu); return cpu_execute_instruction_CB(cpu, opcb) + 4; } //PREFIX CB
        case 0xCC: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, Z_FLAG) == 1) {instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } else { return 12; }} //CALL Z, a16
        case 0xCD: { uint16_t a16 = cpu_fetch_word_pc(cpu); instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } //CALL a16
        case 0xCE: { instr_adc(cpu, cpu_fetch_byte_pc(cpu)); return 8; } //ADC A, n8
        case 0xCF: { instr_push(cpu, cpu->PC); cpu->PC = 0x0008; return 16; } //RST $08

        case 0xD0: { if (cpu_getFlag(cpu, C_FLAG) == 0) { cpu->PC = instr_pop(cpu); return 20; } else { return 8; } } //RET NC
        case 0xD1: { cpu->DE.r16 = instr_pop(cpu); return 12; } //POP DE
        case 0xD2: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 0) { cpu->PC = a16; return 16; } else { return 12; }} //JP NC, a16
        case 0xD4: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 0) { instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } else { return 12; }} //CALL NC, a16
        case 0xD5: { instr_push(cpu, cpu->DE.r16); return 16; } //PUSH DE
        case 0xD6: { instr_sub(cpu, cpu_fetch_byte_pc(cpu)); return 8; } //SUB A, n8
        case 0xD7: { instr_push(cpu, cpu->PC); cpu->PC = 0x0010; return 16; } //RST $10
        case 0xD8: { if (cpu_getFlag(cpu, C_FLAG) == 1) { cpu->PC = instr_pop(cpu); return 20; } else { return 8; } } //RET C
        case 0xD9: { cpu->IME = true; cpu->PC = instr_pop(cpu); return 16; } //RETI (no delay time like ei to enable IME)
        case 0xDA: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 1) { cpu->PC = a16; return 16; } else { return 12; }} //JP C, a16
        case 0xDC: { uint16_t a16 = cpu_fetch_word_pc(cpu); if (cpu_getFlag(cpu, C_FLAG) == 1) { instr_push(cpu, cpu->PC); cpu->PC = a16; return 24; } else { return 12; }} //CALL C, a16
        case 0xDE: { instr_sbc(cpu, cpu_fetch_byte_pc(cpu)); return 8; } //SBC A, n8
        case 0xDF: { instr_push(cpu, cpu->PC); cpu->PC = 0x0018; return 16; } //RST $18

        case 0xE0: { memory_write8(cpu->bus, (0xFF00 + cpu_fetch_byte_pc(cpu)), cpu->AF.r8.hi); return 12; } //LDH (a8), A
        case 0xE1: { cpu->HL.r16 = instr_pop(cpu); return 12; } //POP HL
        case 0xE2: { memory_write8(cpu->bus, (0xFF00 + cpu->BC.r8.lo), cpu->AF.r8.hi); return 8; } //LD (C), A
        case 0xE5: { instr_push(cpu, cpu->HL.r16); return 16; } //PUSH HL
        case 0xE6: { instr_and(cpu, cpu_fetch_byte_pc(cpu)); return 8; } //AND A, n8
        case 0xE7: { instr_push(cpu, cpu->PC); cpu->PC = 0x0020; return 16; } //RST $20
        case 0xE8: {int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); cpu->SP = instr_add16imm(cpu, e8); return 16; }; //ADD SP, e8
        case 0xE9: { cpu->PC = cpu->HL.r16; return 4; } //JP HL
        case 0xEA: { memory_write16(cpu->bus, cpu_fetch_word_pc(cpu), cpu->AF.r8.hi); return 16; } //LD (a16), A
        case 0xEE: { instr_xor(cpu, cpu_fetch_byte_pc(cpu)); return 8; } //XOR A, n8
        case 0xEF: { instr_push(cpu, cpu->PC); cpu->PC = 0x0028; return 16; } //RST $28

        case 0xF0: { cpu->AF.r8.hi = memory_read8(cpu->bus, (0xFF00 + cpu_fetch_byte_pc(cpu))); return 12; } //LDH A, (n8)
        case 0xF1: { cpu->AF.r16 = instr_pop(cpu); return 12; } //POP AF
        case 0xF2: { cpu->AF.r8.hi = memory_read8(cpu->bus, (0xFF00 + cpu->BC.r8.lo)); return 8; } //LD A, (C)
        case 0xF3: { cpu->IME = false; cpu->ei_delay = 0; return 4; } //DI
        case 0xF5: { instr_push(cpu, cpu->AF.r16); return 16; } //PUSH AF
        case 0xF6: { instr_or(cpu, cpu_fetch_byte_pc(cpu)); return 8; } //OR A, n8
        case 0xF7: { instr_push(cpu, cpu->PC); cpu->PC = 0x0030; return 16; } //RST 0x30
        case 0xF8: { int8_t e8 = (int8_t)cpu_fetch_byte_pc(cpu); cpu->HL.r16 = instr_add16imm(cpu, e8); return 12; } //LD HL, SP + e8
        case 0xF9: { cpu->SP = cpu->HL.r16; return 8; } //LD SP, HL
        case 0xFA: { cpu->AF.r8.hi = memory_read8(cpu->bus, cpu_fetch_word_pc(cpu)); return 16; } //LD A, (a16)
        case 0xFB: { cpu->ei_delay = 2; return 4; } //EI
        case 0xFE: { instr_cp(cpu, cpu_fetch_byte_pc(cpu)); return 8; } //CP A, n8
        case 0xFF: { instr_push(cpu, cpu->PC); cpu->PC = 0x0038; return 16; } //RST $38

        default: { fprintf(stderr, "[ERROR] : ILLEGAL INSTRUCTION"); abort(); }
    }
}

uint32_t cpu_execute_instruction_CB(Cpu* cpu, uint8_t opcode)
{
    if (!cpu)
        abort();

    switch (opcode) {
       case 0x00: { cpu->BC.r8.hi = instr_rlc(cpu, cpu->BC.r8.hi); return 8; } //RLC B
       case 0x01: { cpu->BC.r8.lo = instr_rlc(cpu, cpu->BC.r8.lo); return 8; } //RLC C
       case 0x02: { cpu->DE.r8.hi = instr_rlc(cpu, cpu->DE.r8.hi); return 8; } //RLC D
       case 0x03: { cpu->DE.r8.lo = instr_rlc(cpu, cpu->DE.r8.lo); return 8; } //RLC E
       case 0x04: { cpu->HL.r8.hi = instr_rlc(cpu, cpu->HL.r8.hi); return 8; } //RLC H
       case 0x05: { cpu->HL.r8.lo = instr_rlc(cpu, cpu->HL.r8.lo); return 8; } //RLC L
       case 0x06: { memory_write8(cpu->bus, cpu->HL.r16, instr_rlc(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RLC (HL)
       case 0x07: { cpu->AF.r8.hi = instr_rlc(cpu, cpu->AF.r8.hi); return 8; } //RLC A
       case 0x08: { cpu->BC.r8.hi = instr_rrc(cpu, cpu->BC.r8.hi); return 8; } //RRC B
       case 0x09: { cpu->BC.r8.lo = instr_rrc(cpu, cpu->BC.r8.lo); return 8; } //RRC C
       case 0x0A: { cpu->DE.r8.hi = instr_rrc(cpu, cpu->DE.r8.hi); return 8; } //RRC D
       case 0x0B: { cpu->DE.r8.lo = instr_rrc(cpu, cpu->DE.r8.lo); return 8; } //RRC E
       case 0x0C: { cpu->HL.r8.hi = instr_rrc(cpu, cpu->HL.r8.hi); return 8; } //RRC H
       case 0x0D: { cpu->HL.r8.lo = instr_rrc(cpu, cpu->HL.r8.lo); return 8; } //RRC L
       case 0x0E: { memory_write8(cpu->bus, cpu->HL.r16, instr_rrc(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RRC (HL)
       case 0x0F: { cpu->AF.r8.hi = instr_rrc(cpu, cpu->AF.r8.hi); return 8; } //RRC A

       case 0x10: { cpu->BC.r8.hi = instr_rl(cpu, cpu->BC.r8.hi); return 8; } //RL B
       case 0x11: { cpu->BC.r8.lo = instr_rl(cpu, cpu->BC.r8.lo); return 8; } //RL C
       case 0x12: { cpu->DE.r8.hi = instr_rl(cpu, cpu->DE.r8.hi); return 8; } //RL D
       case 0x13: { cpu->DE.r8.lo = instr_rl(cpu, cpu->DE.r8.lo); return 8; } //RL E
       case 0x14: { cpu->HL.r8.hi = instr_rl(cpu, cpu->HL.r8.hi); return 8; } //RL H
       case 0x15: { cpu->HL.r8.lo = instr_rl(cpu, cpu->HL.r8.lo); return 8; } //RL L
       case 0x16: { memory_write8(cpu->bus, cpu->HL.r16, instr_rl(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RL (HL)
       case 0x17: { cpu->AF.r8.hi = instr_rl(cpu, cpu->AF.r8.hi); return 8; } //RL A
       case 0x18: { cpu->BC.r8.hi = instr_rr(cpu, cpu->BC.r8.hi); return 8; } //RR B
       case 0x19: { cpu->BC.r8.lo = instr_rr(cpu, cpu->BC.r8.lo); return 8; } //RR C
       case 0x1A: { cpu->DE.r8.hi = instr_rr(cpu, cpu->DE.r8.hi); return 8; } //RR D
       case 0x1B: { cpu->DE.r8.lo = instr_rr(cpu, cpu->DE.r8.lo); return 8; } //RR E
       case 0x1C: { cpu->HL.r8.hi = instr_rr(cpu, cpu->HL.r8.hi); return 8; } //RR H
       case 0x1D: { cpu->HL.r8.lo = instr_rr(cpu, cpu->HL.r8.lo); return 8; } //RR L
       case 0x1E: { memory_write8(cpu->bus, cpu->HL.r16, instr_rr(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RR (HL)
       case 0x1F: { cpu->AF.r8.hi = instr_rr(cpu, cpu->AF.r8.hi); return 8; } //RR A

       case 0x20:{ cpu->BC.r8.hi = instr_sla(cpu, cpu->BC.r8.hi); return 8; } //SLA B
       case 0x21:{ cpu->BC.r8.lo = instr_sla(cpu, cpu->BC.r8.lo); return 8; } //SLA C
       case 0x22:{ cpu->DE.r8.hi = instr_sla(cpu, cpu->DE.r8.hi); return 8; } //SLA D
       case 0x23:{ cpu->DE.r8.lo = instr_sla(cpu, cpu->DE.r8.lo); return 8; } //SLA E
       case 0x24:{ cpu->HL.r8.hi = instr_sla(cpu, cpu->HL.r8.hi); return 8; } //SLA H
       case 0x25:{ cpu->HL.r8.lo = instr_sla(cpu, cpu->HL.r8.lo); return 8; } //SLA L
       case 0x26:{ memory_write8(cpu->bus, cpu->HL.r16, instr_sla(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SLA (HL)
       case 0x27:{ cpu->AF.r8.hi = instr_sla(cpu, cpu->AF.r8.hi); return 8; } //SLA A
       case 0x28: { cpu->BC.r8.hi = instr_sra(cpu, cpu->BC.r8.hi); return 8; } //SRA B
       case 0x29: { cpu->BC.r8.lo = instr_sra(cpu, cpu->BC.r8.lo); return 8; } //SRA C
       case 0x2A: { cpu->DE.r8.hi = instr_sra(cpu, cpu->DE.r8.hi); return 8; } //SRA D
       case 0x2B: { cpu->DE.r8.lo = instr_sra(cpu, cpu->DE.r8.lo); return 8; } //SRA E
       case 0x2C: { cpu->HL.r8.hi = instr_sra(cpu, cpu->HL.r8.hi); return 8; } //SRA H
       case 0x2D: { cpu->HL.r8.lo = instr_sra(cpu, cpu->HL.r8.lo); return 8; } //SRA L
       case 0x2E: { memory_write8(cpu->bus, cpu->HL.r16, instr_sra(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SRA (HL)
       case 0x2F: { cpu->AF.r8.hi = instr_sra(cpu, cpu->AF.r8.hi); return 8; } //SRA A

       case 0x30: { cpu->BC.r8.hi = instr_swap(cpu, cpu->BC.r8.hi); return 8; } //SWAP B
       case 0x31: { cpu->BC.r8.lo = instr_swap(cpu, cpu->BC.r8.lo); return 8; } //SWAP C
       case 0x32: { cpu->DE.r8.hi = instr_swap(cpu, cpu->DE.r8.hi); return 8; } //SWAP D
       case 0x33: { cpu->DE.r8.lo = instr_swap(cpu, cpu->DE.r8.lo); return 8; } //SWAP E
       case 0x34: { cpu->HL.r8.hi = instr_swap(cpu, cpu->HL.r8.hi); return 8; } //SWAP H
       case 0x35: { cpu->HL.r8.lo = instr_swap(cpu, cpu->HL.r8.lo); return 8; } //SWAP L
       case 0x36: { memory_write8(cpu->bus, cpu->HL.r16, instr_swap(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SWAP (HL)
       case 0x37: { cpu->AF.r8.hi = instr_swap(cpu, cpu->AF.r8.hi); return 8; } //SWAP A
       case 0x38: { cpu->BC.r8.hi = instr_srl(cpu, cpu->BC.r8.hi); return 8; } //SRL B
       case 0x39: { cpu->BC.r8.lo = instr_srl(cpu, cpu->BC.r8.lo); return 8; } //SRL C
       case 0x3A: { cpu->DE.r8.hi = instr_srl(cpu, cpu->DE.r8.hi); return 8; } //SRL D
       case 0x3B: { cpu->DE.r8.lo = instr_srl(cpu, cpu->DE.r8.lo); return 8; } //SRL E
       case 0x3C: { cpu->HL.r8.hi = instr_srl(cpu, cpu->HL.r8.hi); return 8; } //SRL H
       case 0x3D: { cpu->HL.r8.lo = instr_srl(cpu, cpu->HL.r8.lo); return 8; } //SRL L
       case 0x3E: { memory_write8(cpu->bus, cpu->HL.r16, instr_srl(cpu, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SRL (HL)
       case 0x3F: { cpu->AF.r8.hi = instr_srl(cpu, cpu->AF.r8.hi); return 8; } //SRL A

       case 0x40: { instr_bit(cpu, 0, cpu->BC.r8.hi); return 8; } //BIT 0, B
       case 0x41: { instr_bit(cpu, 0, cpu->BC.r8.lo); return 8; } //BIT 0, C
       case 0x42: { instr_bit(cpu, 0, cpu->DE.r8.hi); return 8; } //BIT 0, D
       case 0x43: { instr_bit(cpu, 0, cpu->DE.r8.lo); return 8; } //BIT 0, E
       case 0x44: { instr_bit(cpu, 0, cpu->HL.r8.hi); return 8; } //BIT 0, H
       case 0x45: { instr_bit(cpu, 0, cpu->HL.r8.lo); return 8; } //BIT 0, L
       case 0x46: { instr_bit(cpu, 0, memory_read8(cpu->bus, cpu->HL.r16)); return 12; } //BIT 0, (HL)
       case 0x47: { instr_bit(cpu, 0, cpu->AF.r8.hi); return 8; } //BIT 0, A
       case 0x48: { instr_bit(cpu, 1, cpu->BC.r8.hi); return 8; } //BIT 1, B
       case 0x49: { instr_bit(cpu, 1, cpu->BC.r8.lo); return 8; } //BIT 1, C
       case 0x4A: { instr_bit(cpu, 1, cpu->DE.r8.hi); return 8; } //BIT 1, D
       case 0x4B: { instr_bit(cpu, 1, cpu->DE.r8.lo); return 8; } //BIT 1, E
       case 0x4C: { instr_bit(cpu, 1, cpu->HL.r8.hi); return 8; } //BIT 1, H
       case 0x4D: { instr_bit(cpu, 1, cpu->HL.r8.lo); return 8; } //BIT 1, L
       case 0x4E: { instr_bit(cpu, 1, memory_read8(cpu->bus, cpu->HL.r16)); return 12; } //BIT 1, (HL)
       case 0x4F: { instr_bit(cpu, 1, cpu->AF.r8.hi); return 8; } //BIT 1, A

       case 0x50: { instr_bit(cpu, 2, cpu->BC.r8.hi); return 8; } //BIT 2, B
       case 0x51: { instr_bit(cpu, 2, cpu->BC.r8.lo); return 8; } //BIT 2, C
       case 0x52: { instr_bit(cpu, 2, cpu->DE.r8.hi); return 8; } //BIT 2, D
       case 0x53: { instr_bit(cpu, 2, cpu->DE.r8.lo); return 8; } //BIT 2, E
       case 0x54: { instr_bit(cpu, 2, cpu->HL.r8.hi); return 8; } //BIT 2, H
       case 0x55: { instr_bit(cpu, 2, cpu->HL.r8.lo); return 8; } //BIT 2, L
       case 0x56: { instr_bit(cpu, 2, memory_read8(cpu->bus, cpu->HL.r16)); return 12; } //BIT 2, (HL)
       case 0x57: { instr_bit(cpu, 2, cpu->AF.r8.hi); return 8; } //BIT 2, A
       case 0x58: { instr_bit(cpu, 3, cpu->BC.r8.hi); return 8; } //BIT 3, B
       case 0x59: { instr_bit(cpu, 3, cpu->BC.r8.lo); return 8; } //BIT 3, C
       case 0x5A: { instr_bit(cpu, 3, cpu->DE.r8.hi); return 8; } //BIT 3, D
       case 0x5B: { instr_bit(cpu, 3, cpu->DE.r8.lo); return 8; } //BIT 3, E
       case 0x5C: { instr_bit(cpu, 3, cpu->HL.r8.hi); return 8; } //BIT 3, H
       case 0x5D: { instr_bit(cpu, 3, cpu->HL.r8.lo); return 8; } //BIT 3, L
       case 0x5E: { instr_bit(cpu, 3, memory_read8(cpu->bus, cpu->HL.r16)); return 12; } //BIT 3, (HL)
       case 0x5F: { instr_bit(cpu, 3, cpu->AF.r8.hi); return 8; } //BIT 3, A

       case 0x60: { instr_bit(cpu, 4, cpu->BC.r8.hi); return 8; } //BIT 4, B
       case 0x61: { instr_bit(cpu, 4, cpu->BC.r8.lo); return 8; } //BIT 4, C
       case 0x62: { instr_bit(cpu, 4, cpu->DE.r8.hi); return 8; } //BIT 4, D
       case 0x63: { instr_bit(cpu, 4, cpu->DE.r8.lo); return 8; } //BIT 4, E
       case 0x64: { instr_bit(cpu, 4, cpu->HL.r8.hi); return 8; } //BIT 4, H
       case 0x65: { instr_bit(cpu, 4, cpu->HL.r8.lo); return 8; } //BIT 4, L
       case 0x66: { instr_bit(cpu, 4, memory_read8(cpu->bus, cpu->HL.r16)); return 12; } //BIT 4, (HL)
       case 0x67: { instr_bit(cpu, 4, cpu->AF.r8.hi); return 8; } //BIT 4, A
       case 0x68: { instr_bit(cpu, 5, cpu->BC.r8.hi); return 8; } //BIT 5, B
       case 0x69: { instr_bit(cpu, 5, cpu->BC.r8.lo); return 8; } //BIT 5, C
       case 0x6A: { instr_bit(cpu, 5, cpu->DE.r8.hi); return 8; } //BIT 5, D
       case 0x6B: { instr_bit(cpu, 5, cpu->DE.r8.lo); return 8; } //BIT 5, E
       case 0x6C: { instr_bit(cpu, 5, cpu->HL.r8.hi); return 8; } //BIT 5, H
       case 0x6D: { instr_bit(cpu, 5, cpu->HL.r8.lo); return 8; } //BIT 5, L
       case 0x6E: { instr_bit(cpu, 5, memory_read8(cpu->bus, cpu->HL.r16)); return 12; } //BIT 5, (HL)
       case 0x6F: { instr_bit(cpu, 5, cpu->AF.r8.hi); return 8; } //BIT 5, A

       case 0x70: { instr_bit(cpu, 6, cpu->BC.r8.hi); return 8; } //BIT 6, B
       case 0x71: { instr_bit(cpu, 6, cpu->BC.r8.lo); return 8; } //BIT 6, C
       case 0x72: { instr_bit(cpu, 6, cpu->DE.r8.hi); return 8; } //BIT 6, D
       case 0x73: { instr_bit(cpu, 6, cpu->DE.r8.lo); return 8; } //BIT 6, E
       case 0x74: { instr_bit(cpu, 6, cpu->HL.r8.hi); return 8; } //BIT 6, H
       case 0x75: { instr_bit(cpu, 6, cpu->HL.r8.lo); return 8; } //BIT 6, L
       case 0x76: { instr_bit(cpu, 6, memory_read8(cpu->bus, cpu->HL.r16)); return 12; } //BIT 6, (HL)
       case 0x77: { instr_bit(cpu, 6, cpu->AF.r8.hi); return 8; } //BIT 6, A
       case 0x78: { instr_bit(cpu, 7, cpu->BC.r8.hi); return 8; } //BIT 7, B
       case 0x79: { instr_bit(cpu, 7, cpu->BC.r8.lo); return 8; } //BIT 7, C
       case 0x7A: { instr_bit(cpu, 7, cpu->DE.r8.hi); return 8; } //BIT 7, D
       case 0x7B: { instr_bit(cpu, 7, cpu->DE.r8.lo); return 8; } //BIT 7, E
       case 0x7C: { instr_bit(cpu, 7, cpu->HL.r8.hi); return 8; } //BIT 7, H
       case 0x7D: { instr_bit(cpu, 7, cpu->HL.r8.lo); return 8; } //BIT 7, L
       case 0x7E: { instr_bit(cpu, 7, memory_read8(cpu->bus, cpu->HL.r16)); return 12; } //BIT 7, (HL)
       case 0x7F: { instr_bit(cpu, 7, cpu->AF.r8.hi); return 8; } //BIT 7, A

       case 0x80: { cpu->BC.r8.hi = instr_res(0, cpu->BC.r8.hi); return 8; } //RES 0, B
       case 0x81: { cpu->BC.r8.lo = instr_res(0, cpu->BC.r8.lo); return 8; } //RES 0, C
       case 0x82: { cpu->DE.r8.hi = instr_res(0, cpu->DE.r8.hi); return 8; } //RES 0, D
       case 0x83: { cpu->DE.r8.lo = instr_res(0, cpu->BC.r8.lo); return 8; } //RES 0, E
       case 0x84: { cpu->HL.r8.hi = instr_res(0, cpu->HL.r8.hi); return 8; } //RES 0, H
       case 0x85: { cpu->HL.r8.lo = instr_res(0, cpu->HL.r8.lo); return 8; } //RES 0, L
       case 0x86: { memory_write8(cpu->bus, cpu->HL.r16, instr_res(0, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RES 0, (HL)
       case 0x87: { cpu->AF.r8.hi = instr_res(0, cpu->AF.r8.hi); return 8; } //RES 0, A
       case 0x88: { cpu->BC.r8.hi = instr_res(1, cpu->BC.r8.hi); return 8; } //RES 1, B
       case 0x89: { cpu->BC.r8.lo = instr_res(1, cpu->BC.r8.lo); return 8; } //RES 1, C
       case 0x8A: { cpu->DE.r8.hi = instr_res(1, cpu->DE.r8.hi); return 8; } //RES 1, D
       case 0x8B: { cpu->DE.r8.lo = instr_res(1, cpu->BC.r8.lo); return 8; } //RES 1, E
       case 0x8C: { cpu->HL.r8.hi = instr_res(1, cpu->HL.r8.hi); return 8; } //RES 1, H
       case 0x8D: { cpu->HL.r8.lo = instr_res(1, cpu->HL.r8.lo); return 8; } //RES 1, L
       case 0x8E: { memory_write8(cpu->bus, cpu->HL.r16, instr_res(1, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RES 1, (HL)
       case 0x8F: { cpu->AF.r8.hi = instr_res(1, cpu->AF.r8.hi); return 8; } //RES 1, A

       case 0x90: { cpu->BC.r8.hi = instr_res(2, cpu->BC.r8.hi); return 8; } //RES 2, B
       case 0x91: { cpu->BC.r8.lo = instr_res(2, cpu->BC.r8.lo); return 8; } //RES 2, C
       case 0x92: { cpu->DE.r8.hi = instr_res(2, cpu->DE.r8.hi); return 8; } //RES 2, D
       case 0x93: { cpu->DE.r8.lo = instr_res(2, cpu->BC.r8.lo); return 8; } //RES 2, E
       case 0x94: { cpu->HL.r8.hi = instr_res(2, cpu->HL.r8.hi); return 8; } //RES 2, H
       case 0x95: { cpu->HL.r8.lo = instr_res(2, cpu->HL.r8.lo); return 8; } //RES 2, L
       case 0x96: { memory_write8(cpu->bus, cpu->HL.r16, instr_res(2, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RES 2, (HL)
       case 0x97: { cpu->AF.r8.hi = instr_res(2, cpu->AF.r8.hi); return 8; } //RES 2, A
       case 0x98: { cpu->BC.r8.hi = instr_res(3, cpu->BC.r8.hi); return 8; } //RES 3, B
       case 0x99: { cpu->BC.r8.lo = instr_res(3, cpu->BC.r8.lo); return 8; } //RES 3, C
       case 0x9A: { cpu->DE.r8.hi = instr_res(3, cpu->DE.r8.hi); return 8; } //RES 3, D
       case 0x9B: { cpu->DE.r8.lo = instr_res(3, cpu->BC.r8.lo); return 8; } //RES 3, E
       case 0x9C: { cpu->HL.r8.hi = instr_res(3, cpu->HL.r8.hi); return 8; } //RES 3, H
       case 0x9D: { cpu->HL.r8.lo = instr_res(3, cpu->HL.r8.lo); return 8; } //RES 3, L
       case 0x9E: { memory_write8(cpu->bus, cpu->HL.r16, instr_res(3, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RES 3, (HL)
       case 0x9F: { cpu->AF.r8.hi = instr_res(3, cpu->AF.r8.hi); return 8; } //RES 3, A

       case 0xA0: { cpu->BC.r8.hi = instr_res(4, cpu->BC.r8.hi); return 8; } //RES 4, B
       case 0xA1: { cpu->BC.r8.lo = instr_res(4, cpu->BC.r8.lo); return 8; } //RES 4, C
       case 0xA2: { cpu->DE.r8.hi = instr_res(4, cpu->DE.r8.hi); return 8; } //RES 4, D
       case 0xA3: { cpu->DE.r8.lo = instr_res(4, cpu->BC.r8.lo); return 8; } //RES 4, E
       case 0xA4: { cpu->HL.r8.hi = instr_res(4, cpu->HL.r8.hi); return 8; } //RES 4, H
       case 0xA5: { cpu->HL.r8.lo = instr_res(4, cpu->HL.r8.lo); return 8; } //RES 4, L
       case 0xA6: { memory_write8(cpu->bus, cpu->HL.r16, instr_res(4, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RES 4, (HL)
       case 0xA7: { cpu->AF.r8.hi = instr_res(4, cpu->AF.r8.hi); return 8; } //RES 4, A
       case 0xA8: { cpu->BC.r8.hi = instr_res(5, cpu->BC.r8.hi); return 8; } //RES 5, B
       case 0xA9: { cpu->BC.r8.lo = instr_res(5, cpu->BC.r8.lo); return 8; } //RES 5, C
       case 0xAA: { cpu->DE.r8.hi = instr_res(5, cpu->DE.r8.hi); return 8; } //RES 5, D
       case 0xAB: { cpu->DE.r8.lo = instr_res(5, cpu->BC.r8.lo); return 8; } //RES 5, E
       case 0xAC: { cpu->HL.r8.hi = instr_res(5, cpu->HL.r8.hi); return 8; } //RES 5, H
       case 0xAD: { cpu->HL.r8.lo = instr_res(5, cpu->HL.r8.lo); return 8; } //RES 5, L
       case 0xAE: { memory_write8(cpu->bus, cpu->HL.r16, instr_res(5, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RES 5, (HL)
       case 0xAF: { cpu->AF.r8.hi = instr_res(5, cpu->AF.r8.hi); return 8; } //RES 5, A

       case 0xB0: { cpu->BC.r8.hi = instr_res(6, cpu->BC.r8.hi); return 8; } //RES 6, B
       case 0xB1: { cpu->BC.r8.lo = instr_res(6, cpu->BC.r8.lo); return 8; } //RES 6, C
       case 0xB2: { cpu->DE.r8.hi = instr_res(6, cpu->DE.r8.hi); return 8; } //RES 6, D
       case 0xB3: { cpu->DE.r8.lo = instr_res(6, cpu->BC.r8.lo); return 8; } //RES 6, E
       case 0xB4: { cpu->HL.r8.hi = instr_res(6, cpu->HL.r8.hi); return 8; } //RES 6, H
       case 0xB5: { cpu->HL.r8.lo = instr_res(6, cpu->HL.r8.lo); return 8; } //RES 6, L
       case 0xB6: { memory_write8(cpu->bus, cpu->HL.r16, instr_res(6, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RES 6, (HL)
       case 0xB7: { cpu->AF.r8.hi = instr_res(6, cpu->AF.r8.hi); return 8; } //RES 6, A
       case 0xB8: { cpu->BC.r8.hi = instr_res(7, cpu->BC.r8.hi); return 8; } //RES 7, B
       case 0xB9: { cpu->BC.r8.lo = instr_res(7, cpu->BC.r8.lo); return 8; } //RES 7, C
       case 0xBA: { cpu->DE.r8.hi = instr_res(7, cpu->DE.r8.hi); return 8; } //RES 7, D
       case 0xBB: { cpu->DE.r8.lo = instr_res(7, cpu->BC.r8.lo); return 8; } //RES 7, E
       case 0xBC: { cpu->HL.r8.hi = instr_res(7, cpu->HL.r8.hi); return 8; } //RES 7, H
       case 0xBD: { cpu->HL.r8.lo = instr_res(7, cpu->HL.r8.lo); return 8; } //RES 7, L
       case 0xBE: { memory_write8(cpu->bus, cpu->HL.r16, instr_res(7, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //RES 7, (HL)
       case 0xBF: { cpu->AF.r8.hi = instr_res(7, cpu->AF.r8.hi); return 8; } //RES 7, A

       case 0xC0: { cpu->BC.r8.hi = instr_set(0, cpu->BC.r8.hi); return 8; } //SET 0, B
       case 0xC1: { cpu->BC.r8.lo = instr_set(0, cpu->BC.r8.lo); return 8; } //SET 0, C
       case 0xC2: { cpu->DE.r8.hi = instr_set(0, cpu->DE.r8.hi); return 8; } //SET 0, D
       case 0xC3: { cpu->DE.r8.lo = instr_set(0, cpu->BC.r8.lo); return 8; } //SET 0, E
       case 0xC4: { cpu->HL.r8.hi = instr_set(0, cpu->HL.r8.hi); return 8; } //SET 0, H
       case 0xC5: { cpu->HL.r8.lo = instr_set(0, cpu->HL.r8.lo); return 8; } //SET 0, L
       case 0xC6: { memory_write8(cpu->bus, cpu->HL.r16, instr_set(0, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SET 0, (HL)
       case 0xC7: { cpu->AF.r8.hi = instr_set(0, cpu->AF.r8.hi); return 8; } //SET 0, A
       case 0xC8: { cpu->BC.r8.hi = instr_set(1, cpu->BC.r8.hi); return 8; } //SET 1, B
       case 0xC9: { cpu->BC.r8.lo = instr_set(1, cpu->BC.r8.lo); return 8; } //SET 1, C
       case 0xCA: { cpu->DE.r8.hi = instr_set(1, cpu->DE.r8.hi); return 8; } //SET 1, D
       case 0xCB: { cpu->DE.r8.lo = instr_set(1, cpu->BC.r8.lo); return 8; } //SET 1, E
       case 0xCC: { cpu->HL.r8.hi = instr_set(1, cpu->HL.r8.hi); return 8; } //SET 1, H
       case 0xCD: { cpu->HL.r8.lo = instr_set(1, cpu->HL.r8.lo); return 8; } //SET 1, L
       case 0xCE: { memory_write8(cpu->bus, cpu->HL.r16, instr_set(1, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SET 1, (HL)
       case 0xCF: { cpu->AF.r8.hi = instr_set(1, cpu->AF.r8.hi); return 8; } //SET 1, A
 
       case 0xD0: { cpu->BC.r8.hi = instr_set(2, cpu->BC.r8.hi); return 8; } //SET 2, B
       case 0xD1: { cpu->BC.r8.lo = instr_set(2, cpu->BC.r8.lo); return 8; } //SET 2, C
       case 0xD2: { cpu->DE.r8.hi = instr_set(2, cpu->DE.r8.hi); return 8; } //SET 2, D
       case 0xD3: { cpu->DE.r8.lo = instr_set(2, cpu->BC.r8.lo); return 8; } //SET 2, E
       case 0xD4: { cpu->HL.r8.hi = instr_set(2, cpu->HL.r8.hi); return 8; } //SET 2, H
       case 0xD5: { cpu->HL.r8.lo = instr_set(2, cpu->HL.r8.lo); return 8; } //SET 2, L
       case 0xD6: { memory_write8(cpu->bus, cpu->HL.r16, instr_set(2, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SET 2, (HL)
       case 0xD7: { cpu->AF.r8.hi = instr_set(2, cpu->AF.r8.hi); return 8; } //SET 2, A
       case 0xD8: { cpu->BC.r8.hi = instr_set(3, cpu->BC.r8.hi); return 8; } //SET 3, B
       case 0xD9: { cpu->BC.r8.lo = instr_set(3, cpu->BC.r8.lo); return 8; } //SET 3, C
       case 0xDA: { cpu->DE.r8.hi = instr_set(3, cpu->DE.r8.hi); return 8; } //SET 3, D
       case 0xDB: { cpu->DE.r8.lo = instr_set(3, cpu->BC.r8.lo); return 8; } //SET 3, E
       case 0xDC: { cpu->HL.r8.hi = instr_set(3, cpu->HL.r8.hi); return 8; } //SET 3, H
       case 0xDD: { cpu->HL.r8.lo = instr_set(3, cpu->HL.r8.lo); return 8; } //SET 3, L
       case 0xDE: { memory_write8(cpu->bus, cpu->HL.r16, instr_set(3, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SET 3, (HL)
       case 0xDF: { cpu->AF.r8.hi = instr_set(3, cpu->AF.r8.hi); return 8; } //SET 3, A
 
       case 0xE0: { cpu->BC.r8.hi = instr_set(4, cpu->BC.r8.hi); return 8; } //SET 4, B
       case 0xE1: { cpu->BC.r8.lo = instr_set(4, cpu->BC.r8.lo); return 8; } //SET 4, C
       case 0xE2: { cpu->DE.r8.hi = instr_set(4, cpu->DE.r8.hi); return 8; } //SET 4, D
       case 0xE3: { cpu->DE.r8.lo = instr_set(4, cpu->BC.r8.lo); return 8; } //SET 4, E
       case 0xE4: { cpu->HL.r8.hi = instr_set(4, cpu->HL.r8.hi); return 8; } //SET 4, H
       case 0xE5: { cpu->HL.r8.lo = instr_set(4, cpu->HL.r8.lo); return 8; } //SET 4, L
       case 0xE6: { memory_write8(cpu->bus, cpu->HL.r16, instr_set(4, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SET 4, (HL)
       case 0xE7: { cpu->AF.r8.hi = instr_set(4, cpu->AF.r8.hi); return 8; } //SET 4, A
       case 0xE8: { cpu->BC.r8.hi = instr_set(5, cpu->BC.r8.hi); return 8; } //SET 5, B
       case 0xE9: { cpu->BC.r8.lo = instr_set(5, cpu->BC.r8.lo); return 8; } //SET 5, C
       case 0xEA: { cpu->DE.r8.hi = instr_set(5, cpu->DE.r8.hi); return 8; } //SET 5, D
       case 0xEB: { cpu->DE.r8.lo = instr_set(5, cpu->BC.r8.lo); return 8; } //SET 5, E
       case 0xEC: { cpu->HL.r8.hi = instr_set(5, cpu->HL.r8.hi); return 8; } //SET 5, H
       case 0xED: { cpu->HL.r8.lo = instr_set(5, cpu->HL.r8.lo); return 8; } //SET 5, L
       case 0xEE: { memory_write8(cpu->bus, cpu->HL.r16, instr_set(5, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SET 5, (HL)
       case 0xEF: { cpu->AF.r8.hi = instr_set(5, cpu->AF.r8.hi); return 8; } //SET 5, A
 
       case 0xF0: { cpu->BC.r8.hi = instr_set(6, cpu->BC.r8.hi); return 8; } //SET 6, B
       case 0xF1: { cpu->BC.r8.lo = instr_set(6, cpu->BC.r8.lo); return 8; } //SET 6, C
       case 0xF2: { cpu->DE.r8.hi = instr_set(6, cpu->DE.r8.hi); return 8; } //SET 6, D
       case 0xF3: { cpu->DE.r8.lo = instr_set(6, cpu->BC.r8.lo); return 8; } //SET 6, E
       case 0xF4: { cpu->HL.r8.hi = instr_set(6, cpu->HL.r8.hi); return 8; } //SET 6, H
       case 0xF5: { cpu->HL.r8.lo = instr_set(6, cpu->HL.r8.lo); return 8; } //SET 6, L
       case 0xF6: { memory_write8(cpu->bus, cpu->HL.r16, instr_set(6, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SET 6, (HL)
       case 0xF7: { cpu->AF.r8.hi = instr_set(6, cpu->AF.r8.hi); return 8; } //SET 6, A
       case 0xF8: { cpu->BC.r8.hi = instr_set(7, cpu->BC.r8.hi); return 8; } //SET 7, B
       case 0xF9: { cpu->BC.r8.lo = instr_set(7, cpu->BC.r8.lo); return 8; } //SET 7, C
       case 0xFA: { cpu->DE.r8.hi = instr_set(7, cpu->DE.r8.hi); return 8; } //SET 7, D
       case 0xFB: { cpu->DE.r8.lo = instr_set(7, cpu->BC.r8.lo); return 8; } //SET 7, E
       case 0xFC: { cpu->HL.r8.hi = instr_set(7, cpu->HL.r8.hi); return 8; } //SET 7, H
       case 0xFD: { cpu->HL.r8.lo = instr_set(7, cpu->HL.r8.lo); return 8; } //SET 7, L
       case 0xFE: { memory_write8(cpu->bus, cpu->HL.r16, instr_set(7, memory_read8(cpu->bus, cpu->HL.r16))); return 16; } //SET 7, (HL)
       case 0xFF: { cpu->AF.r8.hi = instr_set(7, cpu->AF.r8.hi); return 8; } //SET 7, A

        default: fprintf(stderr, "[ERROR] : ILLEGAL INSTRUCTION CB");
                    abort();
    }
}