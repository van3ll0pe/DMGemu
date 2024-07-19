#include "memory.h"
#include "cpu.h"
#include "bus.h"
#include "timer.h"


int main(int ac, char** av)
{
    Memory memory;
    memory_init(&memory);

    Bus bus;
    bus_init(&bus, &memory);

    Cpu cpu;
    cpu_init(&cpu);
    cpu_link_bus(&cpu, &bus);

    while (true) {
        handle_interrupts(&cpu); //manage interrupt

        cpu_tick(&cpu);
        //ppu_tick()
        handle_timer(&bus, cpu.cycle);
    }

    return 0;
}