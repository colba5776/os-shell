CC = gcc
CFLAGS = -Wall -Wextra

all: shell task

shell: main.c
	$(CC) $(CFLAGS) -o shell main.c

task: task.c
	$(CC) $(CFLAGS) -o task task.c

clean:
	rm -f shell task
