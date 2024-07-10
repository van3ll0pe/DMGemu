#include "cpu.h"

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