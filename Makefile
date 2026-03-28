CC=gcc
CFLAGS=-Wall -Wextra -std=gnu11

all: myprogram

myprogram: myprogram.c
	$(CC) $(CFLAGS) myprogram.c -o myprogram

clean:
	rm -f myprogram A B C D *.gz result.txt