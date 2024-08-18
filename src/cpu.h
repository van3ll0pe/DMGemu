#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>
#include <stdbool.h>
#include "hard_registers.h"
#include "memory.h"

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

typedef enum
{
    JOYPAD  = 0x10,
    SERIAL  = 0x08,
    TIMER   = 0x04,
    LCD     = 0x02,
    VBLANK  = 0x01
}interrupts;

#define VBLANK_ADDR 0x0040
#define LCD_ADDR    0x0048
#define TIMER_ADDR  0x0050
#define SERIAL_ADDR 0x0058
#define JOYPAD_ADDR 0x0060

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

    bool IME;
    bool is_HALT;
    uint8_t ei_delay;

    Memory* bus;

} Cpu;

void cpu_init(Cpu* cpu, Memory* memory);

uint32_t cpu_execute_instruction(Cpu* cpu, uint8_t opcode);
uint32_t cpu_execute_instruction_CB(Cpu* cpu, uint8_t opcode);

void cpu_update_ei(Cpu* cpu);
void cpu_ticks(Cpu* cpu);


uint8_t cpu_fetch_byte_pc(Cpu* cpu);
uint16_t cpu_fetch_word_pc(Cpu* cpu);

void cpu_setFlag(Cpu* cpu, Flag flag);
void cpu_clearFlag(Cpu* cpu, Flag flag);
void cpu_updateFlag(Cpu* cpu, Flag flag, bool condition);
uint8_t cpu_getFlag(Cpu* cpu, Flag flag);

uint32_t handle_interrupts(Cpu* cpu);



#endif //__CPU_H__