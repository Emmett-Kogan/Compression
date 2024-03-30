CC=gcc
CPP=g++
CFLAGS=-Wall

default: SIM

debug: CFLAGS += -g -D DEBUG
debug: SIM

SIM: main.cpp
	@$(CPP) $(CFLAGS) -o $@ $^

clean:
	@rm -f SIM *.o dout.txt cout.txt
