INC += -I./source/read_line/include
SRC += source/read_line/*.c

INC += -I./source/utilities/include
SRC += source/utilities/*.c

INC += -I./source/llist/include
SRC += source/llist/*.c

all: socket_server.c $(SRC)
	$(CC) $(INC) socket_server.c $(SRC) -w -lpthread -o socket_server.exe

debug_memory: socket_server.c $(SRC)
	$(CC) $(INC) socket_server.c $(SRC) -lpthread -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: socket_server.c $(SRC)
	$(CC) $(INC) socket_server.c $(SRC) -lpthread -o  gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf socket_server debug gdb_debug