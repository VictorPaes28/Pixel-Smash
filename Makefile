# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -I./src  

# Source files
SRCS = $(wildcard src/*.c)

# Output executable
TARGET = main

# Default rule
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET)

# Clean rule to remove the executable
clean:
	rm -f $(TARGET)
