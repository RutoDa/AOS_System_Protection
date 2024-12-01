# Compiler
CC = gcc

# Compiler flags
CFLAGS = -w -pthread

# Files to be compiled
TARGET = server client

# Object files
OBJS = server.o capability_list.o init.o

all: $(TARGET)

server: server.c capability.o init.o
	@echo "Compiling server.c and linking ..."
	@$(CC) $(CFLAGS) -c server.c
	@$(CC) $(CFLAGS) -o server server.o capability.o init.o


capability.o: capability.c capability.h
	@echo "Compiling capability.c ..."
	@$(CC) $(CFLAGS) -c capability.c

init.o: init.c capability.o
	@echo "Compiling init.c ..."
	@$(CC) $(CFLAGS) -c init.c

client: client.c
	@echo "Compiling client.c ..."
	@$(CC) $(CFLAGS) -o client client.c

clean:
	@echo "Cleaning up ..."
	@rm -f $(TARGET) $(OBJS)