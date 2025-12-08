CC = gcc
flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g

all: supervisor generator

supervisor: supervisor.c
	$(CC) $(flags) -o supervisor supervisor.c

generator: generator.c
	$(CC) $(flags) -o generator generator.c

clean:
	rm supervisor generator