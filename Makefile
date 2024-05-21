# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -I./include

# Source files
SRCS = $(wildcard src/*.c)

# Output executable
TARGET = main  # Alterado para gerar o executável main

# Default rule
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET)

# Clean rule to remove the executable
clean:
	rm -f $(TARGET)
