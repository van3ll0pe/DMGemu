#include "serial.h"
#include <stdio.h>
#include <stdlib.h>

static uint8_t serial_output_terminal(char c) {
    fprintf(stdout, "%c", c);
    fflush(stdout);
    return 0;
}

void serial_init(Serial* serial) {
    if (!serial) {exit(1);}

    serial->sb = 0;
    serial->sc = 0;
    serial->interrupt = 0;
}

void serial_write(Serial* serial, uint16_t address, uint8_t data) {
    if (!serial) {exit(1);}

    switch (address) {
        case 0xFF01: {serial->sb = data; break; }    
        case 0xFF02: { serial->sc = (data | 0x7E); 
                        if (data & 0x81) { serial->sb = serial_output_terminal(serial->sb); serial->sb &= ~0x80; serial->interrupt = 0x8; };
                        break; } //get only bit7et bit0 from data, useless bit set to 1
        default: { fprintf(stderr, "Error invalid address for serial"); exit(1); }
    }
}

uint8_t serial_read(Serial* serial, uint16_t address) {
    if (!serial) {exit(1);}

    switch (address) {
        case 0xFF01: { return serial->sb; }
        case 0xFF02: { return serial->sc; }
        default: { fprintf(stderr, "Error invalid address for serial"); exit(1); }
    }
}



/*
void serial_ticks(Serial* serial, uint32_t ticks) {
    if (!serial) {exit(1);}

    if ((serial->sc & 0x81) != 0x81) {return ;} //if gameboy serial is not enable and master

    serial->internal_clock += ticks;
    while(serial->internal_clock >= serial->clock_speed) {
        serial->internal_clock -= serial->clock_speed;
        serial->bit_counter++;
        if (serial->bit_counter == 7) {
            serial->sb = serial_output_terminal(serial->sb);
            serial->bit_counter = 0;
            serial->interrupt_request = true;
            serial->sc &= ~0x80;
        }
    }
}*/