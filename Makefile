CC = gcc
CFLAGS = -O3 -Wall -ansi -pedantic -fmax-errors=1
TARGET = demo

all: main.c
	$(CC) $(CFLAGS) -o $(TARGET) *.c

clean:
	rm $(TARGET)
