#include "memory.h"
#include "cpu.h"
#include "bus.h"


int main(int ac, char** av)
{
    Memory memory;
    memory_init(&memory);

    Bus bus;
    bus_init(&bus, &memory);

    Cpu cpu;
    cpu_init(&cpu);
    cpu_link_bus(&cpu, &bus);
    return 0;
}