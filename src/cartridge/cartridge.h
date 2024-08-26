#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include <stdbool.h>
#include <stdint.h>


typedef struct {
    uint8_t* rom;
    uint8_t* ram;
    long rom_size; //size of rom array
    long ram_size; //size of ram array
    uint8_t mbc_type; //mbc type noMBC, MBC1
    uint8_t current_rom_bank;
    uint8_t current_ram_bank;
    uint32_t nbr_rom_bank; //number of rom banks
    uint32_t nbr_ram_bank; //number of ram banks
    bool ram_enable; //handle external ram activation
    uint8_t mode_flag; //manage read for mbc1
} Cartridge;

void load_cartridge(Cartridge* cartridge,const char* filename);
void eject_cartridge(Cartridge* cartridge);

uint8_t cartridge_read(Cartridge* cartridge, uint16_t address);
void cartridge_write(Cartridge* cartridge, uint16_t address, uint8_t data);

uint8_t cartridge_read_nombc(Cartridge* cartridge, uint16_t address);
uint8_t cartridge_read_mbc1(Cartridge* cartridge, uint16_t address);
void cartridge_write_nombc(Cartridge* cartridge, uint16_t address, uint8_t data);
void cartridge_write_mbc1(Cartridge* cartridge, uint16_t address, uint8_t data);

#endif