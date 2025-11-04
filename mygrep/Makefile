# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Target executable
TARGET = mygrep

# Object files
OBJS = mygrep.o

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile step (mygrep.c depends on grep.h)
mygrep.o: mygrep.c grep.h
	$(CC) $(CFLAGS) -c mygrep.c

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)
