# Makefile for compiling client.c and server.c with Diffie-Hellman on Ubuntu

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -lssl -lcrypto
LIBS = -lssl -lcrypto
# Executables
CLIENT = client
SERVER = server

# Source files
CLIENT_SRC = client.c diffiehellman.c style.c
SERVER_SRC = server.c diffiehellman.c style.c

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC) $(LIBS)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC) $(LIBS)

clean:
	rm -f $(CLIENT) $(SERVER)
