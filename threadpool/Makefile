BUILD=build
SRC=src
INCLUDE=include

CC=gcc
CFLAGS=-Wall -Wextra -std=c2x -pedantic -I$(INCLUDE)

OBJS=main.o queue.o channel.o pool.o wait_group.o

run: build
	./a.out

build: $(OBJS)
	$(CC) $(patsubst %, $(BUILD)/%, $(OBJS))

main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

queue.o: $(SRC)/queue.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

channel.o: $(SRC)/channel.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

pool.o: $(SRC)/pool.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

wait_group.o: $(SRC)/wait_group.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ -c $<

clear:
	rm -f $(BUILD)/*