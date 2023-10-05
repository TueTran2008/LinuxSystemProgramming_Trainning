INC += -I./source/read_line/include
#INC += -I./source/login_accounting/include

#SRC += source/daemon/*.c
SRC += source/read_line/*.c

all: is_seqnum_sv.c $(SRC)
	$(CC) $(INC) is_seqnum_sv.c $(SRC) -lpthread -o is_seqnum_sv.exe

debug_memory: is_seqnum_sv.c $(SRC)
	$(CC) $(INC) is_seqnum_sv.c $(SRC) lpthread -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: is_seqnum_sv.c $(SRC)
	$(CC) $(INC) is_seqnum_sv.c $(SRC) lpthread -o  gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf is_seqnum_sv debug gdb_debug