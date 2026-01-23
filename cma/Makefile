BUILD=build
SRC=src
INCLUDE=include

CC=gcc
CFLAGS=-Wall -Wextra -std=c2x -pedantic -I$(INCLUDE) -g

OBJS=main.o cma_memory.o

run: build
	./a.out

build: $(OBJS)
	$(CC) $(patsubst %, $(BUILD)/%, $(OBJS))

main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

cma_memory.o: $(SRC)/cma_memory.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

clear:
	rm -f $(BUILD)/*