#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>
#include "bus.h"
#include <stdbool.h>

//HARDWARE REGISTER

#define P1 0xFF00
#define SB 0xFF01
#define SC 0xFF02
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07
#define IF 0xFF0F
#define NR10 0xFF10
#define NR11 0xFF11
#define NR12 0xFF12
#define NR13 0xFF13
#define NR14 0xFF14
#define NR21 0xFF16
#define NR22 0xFF17
#define NR23 0xFF18
#define NR24 0xFF19
#define NR30 0xFF1A
#define NR31 0xFF1B
#define NR32 0xFF1C
#define NR33 0xFF1D
#define NR34 0xFF1E
#define NR41 0xFF20
#define NR42 0xFF21
#define NR43 0xFF22
#define NR44 0xFF23
#define NR50 0xFF24
#define NR51 0xFF25
#define NR52 0xFF26
#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define DMA 0xFF46
#define BGP 0xFF47
#define OBP0 0xFF48
#define OBP1 0xFF49
#define WY 0xFF4A
#define WX 0xFF4B
#define KEY1 0xFF4D
#define VBK 0xFF4F
#define HDMA1 0xFF51
#define HDMA2 0xFF52
#define HDMA3 0xFF53
#define HDMA4 0xFF54
#define HDMA5 0xFF55
#define RP 0XFF56
#define BCPS 0xFF68
#define BCPD 0xFF69
#define OCPS 0xFF6A
#define OCPD 0xFF6B
#define SVBK 0xFF70
#define IE 0xFFFF

//FLAG CPU

#define Z_FLAG 0x80
#define N_FLAG 0x40
#define H_FLAG 0x20
#define C_FLAG 0x10

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

    Bus* bus;

} Cpu;

void cpu_init(Cpu* cpu);
void cpu_link_bus(Cpu* cpu, Bus* bus);

void cpu_fetch_instruction(Cpu* cpu);
void cpu_execute_instruction(Cpu* cpu);

uint8_t cpu_getPCImm8(Cpu* cpu);
uint16_t cpu_getPCImm16(Cpu* cpu);

void cpu_setFlag(Cpu* cpu, uint8_t flag);
void cpu_clearFlag(Cpu* cpu, uint8_t flag);
void cpu_checkFlag(Cpu* cpu, uint8_t flag, bool condition);
uint8_t cpu_getFlag(Cpu* cpu, uint8_t flag);



#endif //__CPU_H__