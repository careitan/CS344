CC = gcc
CFLAGS = -lpthread -g -Wall
STD = -std=c99

all: p1

p1: smallsh.c
	$(CC) -o smallsh smallsh.c $(CFLAGS) $(STD)

clean:
	rm smallsh