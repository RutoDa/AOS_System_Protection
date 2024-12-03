# Compiler
CC = gcc

# Compiler flags
CFLAGS = -w -pthread

# Directories
SRCDIR = server
OBJDIR = obj
CLIENTDIR = client

# Files to be compiled
TARGET = server.exe client.exe

# Object files
SERVER_OBJS = $(OBJDIR)/capability.o $(OBJDIR)/init.o $(OBJDIR)/command_handler.o $(OBJDIR)/server.o

all: $(TARGET)

server.exe: $(SERVER_OBJS)
	@echo "Linking server.exe ..."
	@$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/server.o: $(SRCDIR)/server.c
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/capability.o: $(SRCDIR)/capability.c 
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/command_handler.o: $(SRCDIR)/command_handler.c 
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/init.o: $(SRCDIR)/init.c $(OBJDIR)/capability.o
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

client.exe: $(CLIENTDIR)/client.c
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "Cleaning up ..."
	@rm -f $(OBJDIR)/*.o
	@rm -f *.exe