# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c11

# Source files
SRC_FILES = defs.h ghost.c house.c hunter.c loggers.c main.c room.c utils.c

# Executable name
EXEC = FP

# Targets
all: $(SRC_FILES)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC_FILES)

clean:
	rm -f $(EXEC)
