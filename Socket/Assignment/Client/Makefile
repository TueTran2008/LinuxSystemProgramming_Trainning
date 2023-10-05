INC += -I./source/read_line/include
SRC += source/read_line/*.c

all: socket_client.c $(SRC)
	$(CC) $(INC) socket_client.c $(SRC) -o socket_client.exe

debug_memory: socket_client.c $(SRC)
	$(CC) $(INC) socket_client.c $(SRC) -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: socket_client.c $(SRC)
	$(CC) $(INC) socket_client.c $(SRC)  -o  gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf socket_client debug gdb_debug