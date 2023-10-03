INC += -I./source/my_allocation/include
SRC += source/my_allocation/*.c
all: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -o my_heap.exe

debug_memory: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -o gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf main debug gdb_debug