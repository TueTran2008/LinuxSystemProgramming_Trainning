INC += -I./source/time_count/include
SRC += source/time_count/*.c
all: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -lpthread -o timer_signal.exe

debug_memory: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) lpthread -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) lpthread -o  gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf main debug gdb_debug