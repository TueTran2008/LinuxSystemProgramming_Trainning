#INC += -I./source/file_handle/include

#SRC += source/file_handle/*.c

all: sem_thread_sync.c $(SRC)
	$(CC) $(INC) sem_thread_sync.c $(SRC) -lpthread -o sync_thread.exe

debug_memory: sem_thread_sync.c $(SRC)
	$(CC) $(INC) sem_thread_sync.c $(SRC) -lpthread -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: sem_thread_sync.c $(SRC)
	$(CC) $(INC) sem_thread_sync.c $(SRC) -lpthread -o gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf sem_thread_sync debug gdb_debug