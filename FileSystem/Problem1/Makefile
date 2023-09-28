INC += -I./source/file_handle/include
INC += -I./source/file_type/include

SRC += source/file_handle/*.c
SRC += source/file_type/*.c

all: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -o problem1.exe

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