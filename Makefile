CC = gcc
CFLAGS = -o -g3
LDFLAGS = -lncurses

rain: rain.o
rain.o: rain.c
