CC = gcc
CFLAGS = -g -Wall -lSDL -Iinclude

all: c8emu

c8emu: main.o chip8.o video.o 
	$(CC) $(CFLAGS) main.o chip8.o video.o -o c8emu

%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.o c8emu
