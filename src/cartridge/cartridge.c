#include "cartridge.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//return the number of bank ram used. One bank ram is 8KiB
static uint8_t rambank_number(uint8_t data) {
    switch (data) {
        case 0x00: { return 0; }
        case 0x01: { return 1; }
        case 0x02: { return 1; }
        case 0x03: { return 4; }
        case 0x04: { return 16; }
        case 0x05: { return 8; }
        default: { return 0; }
    }
}

static uint8_t rombank_number(uint8_t data) {
    if (data <= 0x8)
        return (2 << data);
    else
        return 1;
}

void load_cartridge(Cartridge* cartridge,const char* filename) {
    if (!cartridge || !filename) { exit(1); }

    FILE* file = fopen(filename, "rb");
    if (!file) { fprintf(stderr, "Error: Cartridge not open"); exit(1); }

    fseek(file, 0, SEEK_END);
    cartridge->rom_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    cartridge->rom = malloc(sizeof(uint8_t) * cartridge->rom_size);
    if (!cartridge->rom) { fclose(file); fprintf(stderr, "Error malloc cartridge"); exit(1); }

    fread(cartridge->rom, 1, cartridge->rom_size, file);
    fclose(file);

    if (cartridge->rom_size < 0x150) { free(cartridge->rom); fprintf(stderr, "Error rom_size is too short"); exit(1); }

    cartridge->mbc_type = cartridge->rom[0x147]; //get the MBC Type of the cartridge
    if (cartridge->mbc_type != 0  && cartridge->mbc_type != 1) { free(cartridge->rom); fprintf(stderr, "Error: MBC cartridge unsupported"); exit(1); } //unsupported MBC type cartridge

    cartridge->nbr_ram_bank = rambank_number(cartridge->rom[0x149]);
    cartridge->nbr_rom_bank = rombank_number(cartridge->rom[0x148]);
    //default constructor
    cartridge->ram = NULL;
    cartridge->ram_enable = false;
    cartridge->ram_size = 0;
    cartridge->current_ram_bank = 0;
    cartridge->current_rom_bank = 1;
    cartridge->mode_flag = 0;

    //allocate ram array if not noMBC but ram_enable still false
    if (cartridge->mbc_type != 0) { //MBC1, etc.
        cartridge->ram_size = cartridge->nbr_ram_bank * 0x2000; //ram size = ram bank number * 8KiB (size of 1 ram bank)
        cartridge->ram = malloc(sizeof(uint8_t) * cartridge->ram_size);
        if (!cartridge->ram) { free(cartridge->rom); fprintf(stderr, "Error ram size"); exit(1); }
        memset(cartridge->ram, 0, sizeof(uint8_t) * cartridge->ram_size);
    }
}

void eject_cartridge(Cartridge* cartridge) {
    if (!cartridge) { exit(1); }

    if (cartridge->rom) { free(cartridge->rom); }
    if (cartridge->ram) { free(cartridge->ram); }
}

uint8_t cartridge_read(Cartridge* cartridge, uint16_t address) {
    if (!cartridge) { exit(1); }

    if (cartridge->mbc_type == 0)
        return cartridge_read_nombc(cartridge, address);
    else
        return cartridge_read_mbc1(cartridge, address);
   return 0xFF;
}

void cartridge_write(Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (!cartridge) { exit(1); }

    if (cartridge->mbc_type == 0)
        cartridge_write_nombc(cartridge, address, data);
    else
        cartridge_write_mbc1(cartridge, address, data);
}


uint8_t cartridge_read_nombc(Cartridge* cartridge, uint16_t address) {
    if (!cartridge || !cartridge->rom) { exit(1); }

    if (address >= 0xA000 && address <= 0xBFFF)
        return 0xFF; //NO RAM in noMBC so return default value
    
    return cartridge->rom[address];
}

void cartridge_write_nombc(Cartridge* cartridge, uint16_t address, uint8_t data) {
    return; //Read Only Memory, not modificition accepted and NO external RAM
}


uint8_t cartridge_read_mbc1(Cartridge* cartridge, uint16_t address) {
    if (!cartridge || !cartridge->rom) { exit(1); }

    if (address < 0x4000) {
        // Lecture dans la première banque (0x0000 - 0x3FFF)
        if (cartridge->mode_flag == 0) {
            return cartridge->rom[address];
        } else {
            uint32_t bank_offset = (cartridge->current_ram_bank << 5) % cartridge->nbr_rom_bank;
            uint32_t rom_address = (bank_offset * 0x4000) + address;
            return cartridge->rom[rom_address];
        }
    } else if (address >= 0x4000 && address < 0x8000) {
        // Lecture dans une autre banque de ROM (0x4000 - 0x7FFF)
        uint32_t bank_number = cartridge->current_rom_bank % cartridge->nbr_rom_bank;
        uint32_t rom_address = (bank_number * 0x4000) + (address - 0x4000);
        return cartridge->rom[rom_address];
    } else if (address >= 0xA000 && address < 0xC000) {
        // Lecture dans la RAM externe (0xA000 - 0xBFFF)
        if (cartridge->ram_enable) {
            uint32_t ram_address = (cartridge->current_ram_bank * 0x2000) + (address - 0xA000);
            return cartridge->ram[ram_address];
        } else {
            return 0xFF; // RAM désactivée
        }
    }
    
    return 0xFF; // Adresse hors de portée
}

void cartridge_write_mbc1(Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (!cartridge) { exit(1); }

    if (address < 0x2000) {
        // Active ou désactive la RAM externe
        if ((data & 0x0F) == 0x0A) {
            cartridge->ram_enable = true;
        } else {
            cartridge->ram_enable = false;
        }
    } else if (address >= 0x2000 && address < 0x4000) {
        // Sélection de la banque ROM (5 bits)
        cartridge->current_rom_bank = data & 0x1F;
        if (cartridge->current_rom_bank == 0) {
            cartridge->current_rom_bank = 1; // La banque 0 ne peut pas être sélectionnée
        }
    } else if (address >= 0x4000 && address < 0x6000) {
        // Sélection de la banque RAM ou bits supplémentaires pour la banque ROM
        cartridge->current_ram_bank = data & 0x03;
    } else if (address >= 0x6000 && address < 0x8000) {
        // Mode de bancage (ROM/RAM)
        cartridge->mode_flag = data & 0x01;
    } else if (address >= 0xA000 && address < 0xC000 && cartridge->ram_enable) {
        // Écriture dans la RAM externe
        uint32_t ram_address = (cartridge->current_ram_bank * 0x2000) + (address - 0xA000);
        cartridge->ram[ram_address] = data;
    }
}

