
all: unix_server.c $(SRC)
	$(CC) $(INC) unix_server.c $(SRC) -o unix_server.exe

debug_memory: unix_server.c $(SRC)
	$(CC) $(INC) unix_server.c $(SRC) -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: unix_server.c $(SRC)
	$(CC) $(INC) unix_server.c $(SRC) -o gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf unix_server debug gdb_debug