
all: i6d_ucase_cl.c $(SRC)
	$(CC) $(INC) i6d_ucase_cl.c $(SRC) -o i6d_ucase_cl.exe

debug_memory: i6d_ucase_cl.c $(SRC)
	$(CC) $(INC) i6d_ucase_cl.c $(SRC) -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: i6d_ucase_cl.c $(SRC)
	$(CC) $(INC) i6d_ucase_cl.c $(SRC) -o gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf i6d_ucase_cl debug gdb_debug