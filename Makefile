CC=gcc
CPP=g++
CFLAGS=-Wall

default: main

debug: CFLAGS += -g -D DEBUG
debug: main

main: main.cpp
	@$(CPP) $(CFLAGS) -o $@ $^

clean:
	@rm -f main *.o dout.txt cout.txt
