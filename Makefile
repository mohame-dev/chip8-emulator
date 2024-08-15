CC = gcc

CFLAGS = -Wall -Wextra -std=c11 `sdl2-config --cflags`

LDFLAGS = `sdl2-config --libs`

TARGET = chip8

SOURCES = $(wildcard *.c)

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
