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
        return (2 << (data));
    else
        return 1;
}

void load_cartridge(Cartridge* cartridge,const char* filename) {
    if (!cartridge || !filename) { abort(); }

    FILE* file = fopen(filename, "rb");
    if (!file) { fprintf(stderr, "Error: Cartridge not open"); abort(); }

    fseek(file, 0, SEEK_END);
    cartridge->rom_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    cartridge->rom = malloc(sizeof(uint8_t) * cartridge->rom_size);
    if (!cartridge->rom) { fclose(file); fprintf(stderr, "Error malloc cartridge"); abort(); }

    fread(cartridge->rom, 1, cartridge->rom_size, file);
    fclose(file);

    if (cartridge->rom_size < 0x150) { free(cartridge->rom); fprintf(stderr, "Error rom_size is too short"); abort(); }

    cartridge->mbc_type = cartridge->rom[0x147]; //get the MBC Type of the cartridge
    if (cartridge->mbc_type > 0x3) { free(cartridge->rom); fprintf(stderr, "Error: MBC cartridge unsupported"); abort(); } //unsupported MBC type cartridge

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
        if (!cartridge->ram) { free(cartridge->rom); fprintf(stderr, "Error ram size"); abort(); }
        memset(cartridge->ram, 0, sizeof(uint8_t) * cartridge->ram_size);
    }
}

void eject_cartridge(Cartridge* cartridge) {
    if (!cartridge) { abort(); }

    if (cartridge->rom) { free(cartridge->rom); }
    if (cartridge->ram) { free(cartridge->ram); }
}

uint8_t cartridge_read(Cartridge* cartridge, uint16_t address) {
    if (!cartridge) { abort(); }

    if (cartridge->mbc_type == 0)
        return cartridge_read_nombc(cartridge, address);
    else
        return cartridge_read_mbc1(cartridge, address);
   return 0xFF;
}

void cartridge_write(Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (!cartridge) { abort(); }

    if (cartridge->mbc_type == 0)
        cartridge_write_nombc(cartridge, address, data);
    else
        cartridge_write_mbc1(cartridge, address, data);
}


uint8_t cartridge_read_nombc(Cartridge* cartridge, uint16_t address) {
    if (!cartridge || !cartridge->rom) { abort(); }

    if (address >= 0xA000 && address <= 0xBFFF)
        return 0xFF; //NO RAM in noMBC so return default value
    
    return cartridge->rom[address];
}

void cartridge_write_nombc(Cartridge* cartridge, uint16_t address, uint8_t data) {
    return; //Read Only Memory, not modificition accepted and NO external RAM
}


uint8_t cartridge_read_mbc1(Cartridge* cartridge, uint16_t address) {
    if (!cartridge || !cartridge->rom) { abort(); }

     if (address >= 0x0000 && address <= 0x3FFF) {
        return cartridge->rom[address];
    }

    if (address >= 0x4000 && address <= 0x7FFF) {
        uint16_t address_into_bank = address - 0x4000;
        uint32_t bank_offset = 0x4000 * cartridge->current_rom_bank;

        uint32_t address_in_rom = bank_offset + address_into_bank;
        return cartridge->rom[address_in_rom];
    }

    if (address >= 0xA000 && address <= 0xBFFF) {
        uint16_t offset_into_ram = 0x2000 * cartridge->current_ram_bank;
        uint16_t address_in_ram = (address - 0xA000) + offset_into_ram;
        return cartridge->ram[address_in_ram];
    }
}

void cartridge_write_mbc1(Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (!cartridge) { abort(); }

     if (address >= 0x0000 && address <= 0x1FFF) {
        cartridge->ram_enable = true;
    }

    if (address >= 0x2000 && address <= 0x3FFF) {
        if (data == 0x0) { cartridge->current_rom_bank = 0x1; }
        if (data == 0x20) { cartridge->current_rom_bank = 0x21; return; }
        if (data == 0x40) { cartridge->current_rom_bank = 0x41; return; }
        if (data == 0x60) { cartridge->current_rom_bank = 0x61; return; }

        uint16_t rom_bank_bits = data & 0x1F;
        cartridge->current_rom_bank = rom_bank_bits;
    }

    if (address >= 0x4000 && address <= 0x5FFF) {
        printf("Unimplemented: Setting upper bits of ROM bank number");
        fflush(stdout);
    }

    if (address >= 0x6000 && address <= 0x7FFF) {
        printf("Unimplemented: Selecting ROM/RAM Mode");
        fflush(stdout);
    }

    if (address >= 0xA000 && address <= 0xBFFF) {
        if (!cartridge->ram_enable) { return; }

        uint16_t offset_into_ram = 0x2000 * cartridge->current_ram_bank;
        uint16_t address_in_ram = (address - 0xA000) + offset_into_ram;
        cartridge->ram[address_in_ram] = data;
    }
}

