INC += -I./source/socket/socket_http/include
SRC += source/socket/socket_http/*.c

INC += -I./source/socket/socket_ip/include
SRC += source/socket/socket_ip/*.c

INC += -I./source/socket/socket_utilities/include
SRC += source/socket/socket_utilities/*.c

INC += -I./source/speedtest/include
SRC += source/speedtest/*.c

INC += -I./source/user_app/old_app_debug
SRC += source/user_app/old_app_debug/*.c

INC += -I./source/user_app/utilities/include
SRC += source/user_app/utilities/*.c

INC += -I./source/openssl/openssl/include

# LIB += -L./source/openssl/openssl
# INC += -I./source/openssl/openssl/include/internal
# INC += -I./source/openssl/openssl/include/openssl

all: main.c $(SRC)
	$(CC) $(INC) main.c  $(SRC) -o speedtest -lpthread -lm ${LIB} -lssl -lcrypto 

debug_memory: main.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -lpthread -std=c11 -lm -o debug
	valgrind --track-origins=yes --leak-check=full ./debug
	rm -rf debug

debug_gdb: main.c.c $(SRC)
	$(CC) $(INC) main.c $(SRC) -lpthread -o  gdb_debug -g
	gdb ./gdb_debug
	rm -rf gdb_debug

clean:
	rm -rf speedtest debug gdb_debug