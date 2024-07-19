#include "timer.h"
#include "hard_registers.h"

void handle_timer(Bus* bus, uint8_t elapsed_Tcycles)
{
    if (!bus)
        exit(1);
    //DIV HARD REGISTER, INCREMENT EACH 256 T CYCLE
    static uint32_t div_t_cycles = 0; //total T cycles for DIV
    static uint32_t tima_t_cycles = 0; //total T cycles for tima
    div_t_cycles += elapsed_Tcycles;
    
    
    uint8_t hr_div = bus->bus_read8(bus->component, DIV);
    if (div_t_cycles >= 256) { //increment
        div_t_cycles -= 256;
        hr_div++;
        bus->bus_write8(bus->component, DIV, hr_div);
    }

    uint8_t hr_tac = bus->bus_read8(bus->component, TAC);
    if (!(hr_tac & 0x04)) //timer disabled
        return;
    
    //timer enabled
    tima_t_cycles += elapsed_Tcycles; //increment the elapsed cycle to increment TIMA register
    uint16_t clock_select = 0;
    switch (hr_tac & 0x03) {
        case 0:     clock_select = 1024;
                    break;
        case 1:     clock_select = 16;
                    break;
        case 2:     clock_select = 64;
                    break;
        case 3:     clock_select = 256;
                    break;
        default: exit(1);
    }
    
    uint8_t hr_tima = bus->bus_read8(bus->component, TIMA);
    uint8_t hr_tma = bus->bus_read8(bus->component, TMA);
    uint8_t hr_if = bus->bus_read8(bus->component, IF); //interrupt request

    if (tima_t_cycles >= clock_select) { //increment tima
        tima_t_cycles -= clock_select;
        if (hr_tima == 0xFF) { //overflow
            hr_if |= 0x04; //set bit 3 of IF register
            bus->bus_write8(bus->component, IF, hr_if); //set in memory the request interrupt timer
            hr_tima = hr_tma;
        } else
            hr_tima++;
        
        bus->bus_write8(bus->component,  TIMA, hr_tima);
    }
}