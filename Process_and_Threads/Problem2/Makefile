INC += -I./source/daemon/include
INC += -I./source/login_accounting/include

SRC += source/daemon/*.c
SRC += source/login_accounting/*.c

all: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -lpthread -o test_daemon.exe

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