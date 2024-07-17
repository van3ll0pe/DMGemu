#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>
#include "bus.h"
#include <stdbool.h>


typedef enum {
    Z_FLAG = 0x80,
    N_FLAG = 0x40,
    H_FLAG = 0x20,
    C_FLAG = 0x10
} Flag;

typedef enum {
    BIT_7 = 0x80,
    BIT_6 = 0x40,
    BIT_5 = 0x20,
    BIT_4 = 0x10,
    BIT_3 = 0x08,
    BIT_2 = 0x04,
    BIT_1 = 0x02,
    BIT_0 = 0x01
} BIT;

typedef union {
    uint16_t r16;
    struct {
        uint8_t lo;
        uint8_t hi;
    } r8;
} Registre;

typedef struct {
    Registre AF;
    Registre BC;
    Registre DE;
    Registre HL;

    uint16_t PC;
    uint16_t SP;

    uint8_t opcode;
    uint8_t cycle;

    bool IME;
    bool HALT;

    Bus* bus;

} Cpu;

void cpu_init(Cpu* cpu);
void cpu_link_bus(Cpu* cpu, Bus* bus);

void cpu_fetch_instruction(Cpu* cpu);
void cpu_execute_instruction(Cpu* cpu);
void cpu_execute_instruction_CB(Cpu* cpu);

uint8_t cpu_getPCImm8(Cpu* cpu);
uint16_t cpu_getPCImm16(Cpu* cpu);

void cpu_setFlag(Cpu* cpu, Flag flag);
void cpu_clearFlag(Cpu* cpu, Flag flag);
void cpu_checkFlag(Cpu* cpu, Flag flag, bool condition);
uint8_t cpu_getFlag(Cpu* cpu, Flag flag);



#endif //__CPU_H__