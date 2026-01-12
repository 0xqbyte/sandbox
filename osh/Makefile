BUILD=build
SRC=src
INCLUDE=include

CC=gcc
CFLAGS=-Wall -Wextra -std=c23 -pedantic -I$(INCLUDE)
BIN=osh

OBJS=osh.o history_buffer.o command.o option.o

build: $(OBJS)
	$(CC) -o $(BIN) $(patsubst %, $(BUILD)/%, $(OBJS))

osh.o: $(SRC)/osh.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

history_buffer.o: $(SRC)/history_buffer.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

command.o: $(SRC)/command.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

option.o: $(SRC)/option.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

clear:
	rm -f $(BIN) $(BUILD)/*
