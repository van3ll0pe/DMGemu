CC= gcc
INCLUDEDIR= -I ./src/ -I ./src/cartridge
LDFLAGS= -lSDL2main -lSDL2
SRC_FILES= src/gameboy.c \
			src/cpu_instr.c \
			src/cpu.c \
			src/joypad.c \
			src/main.c \
			src/memory.c \
			src/serial.c \
			src/timer.c \
			src/cartridge/cartridge.c

OBJ_FILES= $(SRC_FILES:.c=.o)
EXEC= DMGemu
FLAGS= -g

all: $(EXEC)

DMGemu: $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

cpu.o: src/cpu.h src/hard_registers.h src/memory.h src/timer.h src/serial.h src/cartridge/cartridge.h src/joypad.h src/cpu_instr.h
cpu_instr.o: src/cpu.h src/hard_registers.h src/memory.h src/timer.h src/serial.h src/cartridge/cartridge.h src/joypad.h src/cpu_instr.h
gameboy.o: src/gameboy.h src/cpu.h src/hard_registers.h src/memory.h src/timer.h src/serial.h src/cartridge/cartridge.h src/joypad.h
joypad.o: src/joypad.h
main.o: src/gameboy.h src/cpu.h src/hard_registers.h src/memory.h src/timer.h src/serial.h src/cartridge/cartridge.h src/joypad.h
memory.o: src/memory.h src/timer.h src/serial.h src/cartridge/cartridge.h src/joypad.h src/hard_registers.h
serial.o: src/serial.h
timer.o: src/timer.h
cartridge.o: src/cartridge/cartridge.h

%.o: %.c
	$(CC) -o $@ -c $< $(INCLUDEDIR) $(FLAGS)

.PHONY: clean cleanAll

clean:
	rm -rf src/*.o;\
	rm -rf src/cartridge/*.o\

cleanAll:
	rm -rf src/*.o;\
	rm -rf src/cartridge/*.o;\
	rm -rf $(EXEC)




