CC = gcc
CFLAGS = -g -Wall -lSDL -Iinclude -DDEBUG=1

all: c8emu

c8emu: main.o chip8.o video.o 
	$(CC) $(CFLAGS) main.o chip8.o video.o -o c8emu
	rm *.o

%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm c8emu
