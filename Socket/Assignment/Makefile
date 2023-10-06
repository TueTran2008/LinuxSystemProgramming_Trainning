INC += -I./source/read_line/include
SRC += source/read_line/*.c

INC += -I./source/utilities/include
SRC += source/utilities/*.c

INC += -I./source/llist/include
SRC += source/llist/*.c

INC += -I./source/socket_client/include
SRC += source/socket_client/*.c

INC += -I./source/socket_server/include
SRC += source/socket_server/*.c

all: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -lpthread -o my_socket

debug_memory: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -lpthread -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: main.c.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -lpthread -o  gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf main.c debug gdb_debug