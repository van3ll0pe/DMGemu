#include "memory.h"
#include "hard_registers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t bootRom[0x100] = {
    0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
    0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
    0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
    0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
    0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
    0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
    0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
    0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
    0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
    0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
    0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C,
    0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x00, 0x00, 0x23, 0x7D, 0xFE, 0x34, 0x20,
    0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x00, 0x00, 0x3E, 0x01, 0xE0, 0x50
};

void memory_init(Memory* memory, Serial* serial, Timer* timer, Joypad* joypad, Cartridge* cartridge)
{
    if (!memory || !serial || !timer || !joypad || !cartridge) {
        fprintf(stderr, "[ERROR]: memory initialization failed from structure element");
        exit(1);
    }

    memory->joypad = joypad;
    memory->timer = timer;
    memory->serial = serial;
    memory->cartridge = cartridge;
    memory->disable_bootrom = 0x00;
    memory->interrupt_enable = 0xFF;
    memory->interrupt_requested = 0xE1;

    memset(memory->high_ram, 0, sizeof(uint8_t) * HIGHRAM_SIZE);
    memset(memory->oam_ram, 0, sizeof(uint8_t) * OAMRAM_SIZE);
    memset(memory->work_ram, 0, sizeof(uint8_t) * WORKRAM_SIZE);

    memory_write8(memory, P1, 0xCF);
    memory_write8(memory, SC, 0x7E);
    memory_write8(memory, DIV, 0xAB);
    memory_write8(memory, TAC, 0xF8);
    memory_write8(memory, IF, 0xE1);
    memory_write8(memory, NR10, 0x80);
    memory_write8(memory, NR11, 0xBF);
    memory_write8(memory, NR12, 0xF3);
    memory_write8(memory, NR13, 0xFF);
    memory_write8(memory, NR14, 0xBF);
    memory_write8(memory, NR21, 0x3F);
    memory_write8(memory, NR23, 0xFF);
    memory_write8(memory, NR24, 0xBF);
    memory_write8(memory, NR30, 0x7F);
    memory_write8(memory, NR31, 0xFF);
    memory_write8(memory, NR32, 0x9F);
    memory_write8(memory, NR33, 0xFF);
    memory_write8(memory, NR34, 0xBF);
    memory_write8(memory, NR41, 0xFF);
    memory_write8(memory, NR44, 0xBF);
    memory_write8(memory, NR50, 0x77);
    memory_write8(memory, NR51, 0xF3);
    memory_write8(memory, NR52, 0xF1);
    memory_write8(memory, LCDC, 0x91);
    memory_write8(memory, STAT, 0x85);
    memory_write8(memory, DMA, 0xFF);
    memory_write8(memory, BGP, 0xFC);
    memory_write8(memory, IE, 0xFF);
}

uint8_t memory_read8(Memory* memory, uint16_t address)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    if (address >= 0x0 && address <= 0xFF && !memory->disable_bootrom)
        return bootRom[address];

    switch (address & 0xF000) {
        //ROM from cartridge
        case 0x0000:
        case 0x1000:
        case 0x2000:
        case 0x3000:
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000: { return cartridge_read(memory->cartridge, address); }

        //VRAM ppu
        case 0x8000:
        case 0x9000: { return 0; } //TODO ppu_read(ppu, address)

        //EXTERNAL RAM from cartridge
        case 0xA000:
        case 0xB000: { return cartridge_read(memory->cartridge, address); }

        //WORK RAM
        case 0xC000:
        case 0xD000: { return memory->work_ram[address & 0x1FFF]; }
        
        //echo work ram
        case 0xE000: { return memory->work_ram[address & 0x1FFF]; }

        case 0xF000: switch (address & 0x0F00) {
            //echo work ram
            case 0x000:
            case 0x100:
            case 0x200:
            case 0x300:
            case 0x400:
            case 0x500:
            case 0x600:
            case 0x700:
            case 0x800:
            case 0x900:
            case 0xA00:
            case 0xB00:
            case 0xC00:
            case 0xD00: { return memory->work_ram[address & 0x1FFF]; }

            case 0xE00: switch (address & 0x00F0) {
                //OAM
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                case 0x40:
                case 0x50:
                case 0x60:
                case 0x70:
                case 0x80:
                case 0x90: { return memory->oam_ram[address - 0xFE00]; }

                //FEA0 - FEFF range prohibited
                default: { return 0xFF; }
            }

            case 0xF00: {
                if ((address & 0xFF) >= 0x00 && (address & 0xFF) <= 0x7F) { //I/O registers
                    if (address == 0xFF00) { return joypad_read(memory->joypad, address); }
                    else if ((address >= 0xFF01) && (address <= 0xFF02)) { return serial_read(memory->serial, address); }
                    else if ((address >= 0xFF04) && (address <= 0xFF07)) { return timer_read(memory->timer, address); }
                    else if (address == 0xFF0F) { return memory->interrupt_requested; }
                    else if ((address >= 0xFF40) && (address <= 0xFF4B)) { return 0; } //TODO return ppu_read(ppu, address)}
                    else if (address == 0xFF50) { return memory->disable_bootrom; }
                    else { return 0xFF; }
                } 
                else if ((address & 0xFF) >= 0x80 && (address & 0xFF) <= 0xFE)  { return memory->high_ram[address & 0x7F]; } //high ram
                else { return memory->interrupt_enable; } //IE register
            }

            default: { return 0xFF; }
        }
        default: { return 0xFF; }
    }
}

void memory_write8(Memory* memory, uint16_t address, uint8_t data)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    if (address >= 0x0 && address <= 0xFF && !memory->disable_bootrom)
        return;
    
    switch (address & 0xF000) {
        //ROM from cartridge
        case 0x0000:
        case 0x1000:
        case 0x2000:
        case 0x3000:
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000: { cartridge_write(memory->cartridge, address, data); }

        //VRAM ppu
        case 0x8000:
        case 0x9000: { } //TODO ppu_write(ppu, address)

        //EXTERNAL RAM from cartridge
        case 0xA000:
        case 0xB000: { cartridge_write(memory->cartridge, address, data); }

        //WORK RAM
        case 0xC000:
        case 0xD000: { memory->work_ram[address & 0x1FFF] = data; }
        
        //echo work ram
        case 0xE000: { memory->work_ram[address & 0x1FFF] = data; }

        case 0xF000: switch (address & 0x0F00) {
            //echo work ram
            case 0x000:
            case 0x100:
            case 0x200:
            case 0x300:
            case 0x400:
            case 0x500:
            case 0x600:
            case 0x700:
            case 0x800:
            case 0x900:
            case 0xA00:
            case 0xB00:
            case 0xC00:
            case 0xD00: { memory->work_ram[address & 0x1FFF] = data; }

            case 0xE00: switch (address & 0x00F0) {
                //OAM
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                case 0x40:
                case 0x50:
                case 0x60:
                case 0x70:
                case 0x80:
                case 0x90: { memory->oam_ram[address - 0xFE00] = data; }

                //FEA0 - FEFF range prohibited
                default: { return; }
            }

            case 0xF00: {
                if ((address & 0xFF) >= 0x00 && (address & 0xFF) <= 0x7F) { //I/O registers
                    if (address == 0xFF00) { joypad_write(memory->joypad, address, data); }
                    else if ((address >= 0xFF01) && (address <= 0xFF02)) { serial_write(memory->serial, address, data); }
                    else if ((address >= 0xFF04) && (address <= 0xFF07)) { timer_write(memory->timer, address, data); }
                    else if (address == 0xFF0F) { memory->interrupt_requested = (data | 0xE0); }
                    else if ((address >= 0xFF40) && (address <= 0xFF4B)) {  } //TODO return ppu_write(ppu, address)}
                    else if (address == 0xFF50) { memory->disable_bootrom = data; }
                    else { return; }
                } 
                else if ((address & 0xFF) >= 0x80 && (address & 0xFF) <= 0xFE)  { memory->high_ram[address & 0x7F] = data; } //high ram
                else { memory->interrupt_enable = (data | 0xE0); } //IE register
            }

            default: { return; }
        }
        default: { return; }
    }
    
}

uint16_t memory_read16(Memory* memory, uint16_t address)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    uint16_t data = (memory_read8(memory, (address + 1)) << 8) | memory_read8(memory, address); //little endian
    return data;
}

void memory_write16(Memory* memory,uint16_t address, uint16_t data)
{
    if (!memory) {
        fprintf(stderr, "[ERROR]: memory structure is NULL");
        exit(1);
    }

    memory_write8(memory, address, (data & 0xFF));
    memory_write8(memory, (address + 1), ((data >> 8) & 0xFF));
}