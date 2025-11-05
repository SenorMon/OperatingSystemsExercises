# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g -c
LDFLAGS = 

# Target executable
TARGET = mygrep

# Object files
OBJS = mygrep.o

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

# Compile step (mygrep.c depends on grep.h)
mygrep.o: mygrep.c grep.h
	$(CC) $(CFLAGS) mygrep.c

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)
