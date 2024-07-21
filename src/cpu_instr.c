#include "cpu.h"
#include <stdlib.h>

/**************** 8bit arithmetic and logic instructions **********************/

void cpu_instr_ADC(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);

    uint8_t flag = cpu_getFlag(cpu, C_FLAG);
    uint32_t res = cpu->AF.r8.hi + value + flag;
    uint8_t resFinal = (uint8_t)res;

    cpu_checkFlag(cpu, Z_FLAG, (resFinal == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->AF.r8.hi & 0xF) + (value & 0xF) + flag) & 0x10));
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

    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->HL.r16 & 0xFFF) + (value & 0xFFF)) & 0x1000));
    cpu_checkFlag(cpu, C_FLAG, (res & 0x10000));

    cpu->HL.r16 = (uint16_t)res;
}

void cpu_instr_ADDe8(Cpu* cpu, int8_t value)
{
    if (!cpu)
        exit(1);

    uint32_t res = cpu->SP + value;

    cpu_clearFlag(cpu, Z_FLAG);
    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->SP & 0xF) + (value & 0xF)) & 0x10));
    cpu_checkFlag(cpu, C_FLAG, (res & 0x100));

    cpu->SP = (uint16_t)res;
}

void cpu_instr_AND(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);
    
    cpu->AF.r8.hi &= value;

    cpu_checkFlag(cpu, Z_FLAG, (cpu->AF.r8.hi == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_setFlag(cpu, H_FLAG);
    cpu_clearFlag(cpu, C_FLAG);
}

void cpu_instr_CP(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);
    
    uint8_t res = cpu->AF.r8.hi - value;

    cpu_checkFlag(cpu, Z_FLAG, (res == 0));
    cpu_setFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->AF.r8.hi & 0xF) - (value & 0xF)) & 0x10));
    cpu_checkFlag(cpu, C_FLAG, (cpu->AF.r8.hi < value));
}

void cpu_instr_DEC8(Cpu* cpu, uint8_t* value)
{
    if (!cpu)
        exit(1);
    
    uint8_t res = (*value) - 1;

    cpu_checkFlag(cpu, Z_FLAG, (res == 0));
    cpu_setFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, ((((*value) & 0xF) - 1) & 0x10));

    *value = res;
}

void cpu_instr_DECHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);

    uint8_t res = cpu->bus->bus_read8(cpu->bus->component, address) - 1;

    cpu_checkFlag(cpu, Z_FLAG, (res == 0));
    cpu_setFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, ((((cpu->bus->bus_read8(cpu->bus->component, address)) & 0xF) - 1) & 0x10));

    cpu->bus->bus_write8(cpu->bus->component, address, res);
}

void cpu_instr_DEC16(Cpu* cpu, uint16_t* value)
{
    if (!cpu)
        exit(1);

    (*value)--;

    //flags not affected
}

void cpu_instr_INC8(Cpu* cpu, uint8_t* value)
{
    if (!cpu)
        exit(1);
    
    uint8_t res = (*value) + 1;

    cpu_checkFlag(cpu, Z_FLAG, (res == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, ((((*value) & 0xF) + 1) & 0x10));

    *value = res;
}

void cpu_instr_INCHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);

    uint8_t res = cpu->bus->bus_read8(cpu->bus->component, address) + 1;

    cpu_checkFlag(cpu, Z_FLAG, (res == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, ((((cpu->bus->bus_read8(cpu->bus->component, address)) & 0xF) + 1) & 0x10));

    cpu->bus->bus_write8(cpu->bus->component, address, res);
}

void cpu_instr_INC16(Cpu* cpu, uint16_t *value)
{
    if (!cpu)
        exit(1);
    
    (*value)++;

    //flags not affected
}

void cpu_instr_OR(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);
    
    cpu->AF.r8.hi |= value;

    cpu_checkFlag(cpu, Z_FLAG, (cpu->AF.r8.hi == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_clearFlag(cpu, C_FLAG);
}

void cpu_instr_SBC(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);
    uint8_t flag = cpu_getFlag(cpu, C_FLAG);
    uint32_t res = cpu->AF.r8.hi - value - flag;
    uint8_t resFinal = (uint8_t)res;

    cpu_checkFlag(cpu, Z_FLAG, (resFinal == 0));
    cpu_setFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->AF.r8.hi & 0xf) - (value & 0xf) - flag) & 0x10));
    cpu_checkFlag(cpu, C_FLAG, (res & 0x100));

    cpu->AF.r8.hi = resFinal;
}

void cpu_instr_SUB(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);
    
    uint32_t res = cpu->AF.r8.hi - value;
    uint8_t resFinal = (uint8_t)res;

    cpu_checkFlag(cpu, Z_FLAG, (resFinal == 0));
    cpu_setFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->AF.r8.hi & 0xf) - (value & 0xf)) & 0x10));
    cpu_checkFlag(cpu, C_FLAG, (res & 0x100));

    cpu->AF.r8.hi = resFinal;
}

void cpu_instr_XOR(Cpu* cpu, uint8_t value)
{
    if (!cpu)
        exit(1);
    
    cpu->AF.r8.hi ^= value;

    cpu_checkFlag(cpu, Z_FLAG, (cpu->AF.r8.hi == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_clearFlag(cpu, C_FLAG);
}



/*************************************************************************************/

void cpu_instr_LDr8_8(uint8_t* dst, uint8_t value)
{
    if (!dst)
        exit(1);

    *dst = value;
}

void cpu_instr_LDmem16_8(Cpu* cpu,uint16_t address, uint8_t value)
{
    if (!cpu)
        exit(1);
    
    cpu->bus->bus_write8(cpu->bus->component, address, value);
}

/***********************************************************************************/

void cpu_instr_LDr16_16(uint16_t* dst, uint16_t value)
{
    if (!dst)
        exit(1);
    
    *dst = value;
}

void cpu_instr_LDmem16_16(Cpu* cpu, uint16_t address, uint16_t value)
{
    if (!cpu)
        exit(1);
    
    cpu->bus->bus_write16(cpu->bus->component, address, value);
}

void cpu_instr_LDHLSPe8(Cpu* cpu, int8_t value)
{
    if (!cpu)
        exit(1);
    
    uint32_t res = cpu->SP + value;

    cpu_clearFlag(cpu, Z_FLAG);
    cpu_clearFlag(cpu, N_FLAG);
    cpu_checkFlag(cpu, H_FLAG, (((cpu->SP & 0xf) + (value & 0xf)) & 0x10));
    cpu_checkFlag(cpu, C_FLAG, (res & 0x100));

    cpu->HL.r16 = (uint16_t)res;
}


/************************************************************************************/

void cpu_instr_PUSH(Cpu* cpu, uint16_t r16)
{
    if (!cpu)
        exit(1);
    
    cpu->SP-=2;
    cpu->bus->bus_write16(cpu->bus->component, cpu->SP, r16);
}

void cpu_instr_POP(Cpu* cpu, uint16_t* r16)
{
    if (!cpu || !r16)
        exit(1);
    
    *r16 = cpu->bus->bus_read16(cpu->bus->component, cpu->SP);
    cpu->SP+=2;
}

/**********************************************************************************/

void cpu_instr_CCF(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint8_t new_carry = cpu_getFlag(cpu, C_FLAG) ^ 1; //inverted value

    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, new_carry); //set inverted carry value
}

void cpu_instr_SCF(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_setFlag(cpu, C_FLAG);
}

void cpu_instr_CPL(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    cpu->AF.r8.hi = ~(cpu->AF.r8.hi);

    cpu_setFlag(cpu, N_FLAG);
    cpu_setFlag(cpu, H_FLAG);
}

void cpu_instr_EI(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    cpu->IME = true; 
}

void cpu_instr_DI(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    cpu->IME = false;
}

uint8_t cpu_instr_JP(Cpu* cpu, bool condition, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    if (condition) {
        cpu->PC = address;
        return 16;
    }
    //no jump
    return 12;
}

uint8_t cpu_instr_CALL(Cpu* cpu, bool condition, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    if (condition) {
        cpu_instr_PUSH(cpu, cpu->PC);
        cpu->PC = address;
        return 24;
    }
    //no jump
    return 12;
}

uint8_t cpu_instr_RET(Cpu* cpu, bool condition)
{
    if (!cpu)
        exit(1);
    
    if (condition) {
        cpu_instr_POP(cpu, &cpu->PC);
        return 20;
    }
    //no ret
    return 8;
}

uint8_t cpu_instr_JR(Cpu* cpu, bool condition, int8_t value)
{
    if (!cpu)
        exit(1);
    
    if (condition) {
        cpu->PC += value;
        return 12;
    }
    //no relative jump
    return 8;
}

void cpu_instr_RST(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    cpu_instr_PUSH(cpu, cpu->PC);
    cpu->PC = address;
}

void cpu_instr_RLCA(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint8_t bit7 = (cpu->AF.r8.hi & 0x80) >> 7;
    cpu->AF.r8.hi <<= 1;
    cpu->AF.r8.hi |= bit7;

    cpu_clearFlag(cpu, Z_FLAG);
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit7));
}

void cpu_instr_RRCA(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint8_t bit0 = (cpu->AF.r8.hi & 0x1) << 7;
    cpu->AF.r8.hi >>= 1;

    cpu->AF.r8.hi |= bit0;

    cpu_clearFlag(cpu, Z_FLAG);
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit0));
}

void cpu_instr_RLA(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint8_t bit7 = cpu->AF.r8.hi & 0x80;
    cpu->AF.r8.hi <<= 1;
    cpu->AF.r8.hi |= cpu_getFlag(cpu, C_FLAG); //set the bit0 with C_FLAG value

    cpu_clearFlag(cpu, Z_FLAG);
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit7));
}

void cpu_instr_RRA(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    uint8_t bit0 = cpu->AF.r8.hi & 0x1;
    cpu->AF.r8.hi >>= 1;
    cpu->AF.r8.hi |= (cpu_getFlag(cpu, C_FLAG) << 7);

    cpu_clearFlag(cpu, Z_FLAG);
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit0));
}

void cpu_instr_DAA(Cpu* cpu)
{
    if (!cpu)
        exit(1);
    
    if (!cpu_getFlag(cpu, N_FLAG)) { //last operation is addition
        if (cpu_getFlag(cpu, C_FLAG) || cpu->AF.r8.hi > 0x99) {
            cpu->AF.r8.hi  += 0x60;
            cpu_setFlag(cpu, C_FLAG);
        }
        if (cpu_getFlag(cpu, H_FLAG) || (cpu->AF.r8.hi & 0xf) > 0x09) {
            cpu->AF.r8.hi += 0x6;
        }
    } else { //last operation is substraction
         if (cpu_getFlag(cpu, C_FLAG)) {
            cpu->AF.r8.hi -= 0x60;
        }
        if (cpu_getFlag(cpu, H_FLAG)) {
            cpu->AF.r8.hi -= 0x6;
        }
    }

    cpu_checkFlag(cpu, Z_FLAG, (cpu->AF.r8.hi == 0));
    cpu_clearFlag(cpu, H_FLAG);
}

void cpu_instr_RLC(Cpu* cpu, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);
    
    uint8_t bit7 = ((*r8) & 0x80) >> 7;
    (*r8) <<= 1;
    (*r8) |= bit7;

    cpu_checkFlag(cpu, Z_FLAG, (*r8 == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit7));

}

void cpu_instr_RLCHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    uint8_t bit7 = (value & 0x80) >> 7;
    value <<= 1;
    value |= bit7;

    cpu->bus->bus_write8(cpu->bus->component, address, value);
    cpu_checkFlag(cpu, Z_FLAG, (value == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit7));
}

void cpu_instr_RRC(Cpu* cpu, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);

    uint8_t bit0 = ((*r8) & 0x1) << 7;
    (*r8) >>= 1;
    (*r8) |= bit0;

    cpu_checkFlag(cpu, Z_FLAG, (*r8 == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit0));
}

void cpu_instr_RRCHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    uint8_t bit0 = (value & 0x1) << 7;
    value >>= 1;
    value |= bit0;
    cpu->bus->bus_write8(cpu->bus->component, address, value);

    cpu_checkFlag(cpu, Z_FLAG, (value == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit0));
}

void cpu_instr_RL(Cpu* cpu, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);
    
    uint8_t bit7 = (*r8) & 0x80;
    (*r8) <<= 1;
    (*r8) |= cpu_getFlag(cpu, C_FLAG); //set the bit0 with C_FLAG value

    cpu_checkFlag(cpu, Z_FLAG, (*r8 == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit7));
}

void cpu_instr_RLHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    uint8_t bit7 = value & 0x80;
    value <<= 1;
    value |= cpu_getFlag(cpu, C_FLAG); //set the bit0 with C_FLAG value
    cpu->bus->bus_write8(cpu->bus->component, address, value);

    cpu_checkFlag(cpu, Z_FLAG, (value == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit7));
}

void cpu_instr_RR(Cpu* cpu, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);
    
    uint8_t bit0 = (*r8) & 0x1;
    (*r8) >>= 1;
    (*r8) |= (cpu_getFlag(cpu, C_FLAG) << 7);

    cpu_checkFlag(cpu, Z_FLAG, (*r8 == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit0));
}

void cpu_instr_RRHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);

    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    uint8_t bit0 = value & 0x1;
    value >>= 1;
    value |= (cpu_getFlag(cpu, C_FLAG) << 7);
    cpu->bus->bus_write8(cpu->bus->component, address, value);

    cpu_checkFlag(cpu, Z_FLAG, (value == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit0));
}

void cpu_instr_SLA(Cpu* cpu, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);
    
    uint8_t bit7 = (*r8) & 0x80;
    (*r8) <<= 1; //bit 0 is set to 0

    cpu_checkFlag(cpu, Z_FLAG, (*r8 == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit7));
}

void cpu_instr_SLAHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);

    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    uint8_t bit7 = value & 0x80;
    value <<= 1; //bit 0 is set to 0
    cpu->bus->bus_write8(cpu->bus->component, address, value);

    cpu_checkFlag(cpu, Z_FLAG, (value == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit7));
}

void cpu_instr_SRA(Cpu* cpu, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);
    
    uint8_t bit7 = (*r8) & 0x80;
    uint8_t bit0 = (*r8) & 0x01;

    (*r8) >>= 1;
    (*r8) |= bit7; //the bit7 keeps his value

    cpu_checkFlag(cpu, Z_FLAG, (*r8 == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG,  (bit0));
}

void cpu_instr_SRAHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    uint8_t bit7 = value & 0x80;
    uint8_t bit0 = value & 0x01;

    value >>= 1;
    value |= bit7; //the bit7 keeps his value
    cpu->bus->bus_write8(cpu->bus->component, address, value);

    cpu_checkFlag(cpu, Z_FLAG, (value == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG,  (bit0));
}

void cpu_instr_SWAP(Cpu* cpu, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);
    
    uint8_t upper_nybble = ((*r8) & 0xF0) >> 4;
    (*r8) <<= 4;
    (*r8) |= upper_nybble;

    cpu_checkFlag(cpu, Z_FLAG, (*r8 == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_clearFlag(cpu, C_FLAG);
}

void cpu_instr_SWAPHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    uint8_t upper_nybble = (value & 0xF0) >> 4;
    value <<= 4;
    value |= upper_nybble;
    cpu->bus->bus_write8(cpu->bus->component, address, value);

    cpu_checkFlag(cpu, Z_FLAG, (value == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_clearFlag(cpu, C_FLAG);
}

void cpu_instr_SRL(Cpu* cpu, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);
    
    uint8_t bit0 = (*r8) & 0x01;
    (*r8) >>= 1;

    cpu_checkFlag(cpu, Z_FLAG, (*r8 == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit0));
}

void cpu_instr_SRLHL(Cpu* cpu, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    uint8_t bit0 = value & 0x01;
    value >>= 1;
    cpu->bus->bus_write8(cpu->bus->component, address, value);

    cpu_checkFlag(cpu, Z_FLAG, (value == 0));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_clearFlag(cpu, H_FLAG);
    cpu_checkFlag(cpu, C_FLAG, (bit0));
}

void cpu_instr_BIT(Cpu* cpu, BIT bit, uint8_t value)
{
    if (!cpu)
        exit(1);
    
    uint8_t bit_check = value & bit;

    cpu_checkFlag(cpu, Z_FLAG, (!bit_check));
    cpu_clearFlag(cpu, N_FLAG);
    cpu_setFlag(cpu, H_FLAG);
}

void cpu_instr_RES(Cpu* cpu, BIT bit, uint8_t* r8)
{
    if (!cpu || !r8)
        exit(1);
    
    (*r8) &= ~bit;
}

void cpu_instr_RESHL(Cpu* cpu, BIT bit, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, cpu->HL.r16);
    value &= ~bit;
    cpu->bus->bus_write8(cpu->bus->component, cpu->HL.r16, value);
}

void cpu_instr_SET(Cpu* cpu, BIT bit, uint8_t* r8)
{
    if  (!cpu || !r8)
        exit(1);
    
    (*r8) |= bit;
}

void cpu_instr_SETHL(Cpu* cpu, BIT bit, uint16_t address)
{
    if (!cpu)
        exit(1);
    
    uint8_t value = cpu->bus->bus_read8(cpu->bus->component, address);
    value |= bit;
    cpu->bus->bus_write8(cpu->bus->component, address, value);
}