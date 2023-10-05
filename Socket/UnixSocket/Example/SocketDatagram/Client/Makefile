
all: unix_client.c $(SRC)
	$(CC) $(INC) unix_client.c $(SRC) -o unix_client.exe

debug_memory: unix_client.c $(SRC)
	$(CC) $(INC) unix_client.c $(SRC) -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: unix_client.c $(SRC)
	$(CC) $(INC) unix_client.c $(SRC) -o gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf unix_client debug gdb_debug