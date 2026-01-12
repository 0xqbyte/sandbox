BUILD=build
SRC=src
INCLUDE=include

CC=gcc
CFLAGS=-Wall -Wextra -std=c23 -pedantic -I$(INCLUDE)
BIN=server

OBJS=server-thread-pool.o queue.o channel.o

build: $(OBJS)
	$(CC) -o $(BIN) $(patsubst %, $(BUILD)/%, $(OBJS))

server-thread-pool.o: server-thread-pool.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

queue.o: queue.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

channel.o: channel.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

clear:
	rm -f $(BIN) $(BUILD)/*
