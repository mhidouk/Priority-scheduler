CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: priority_scheduler

priority_scheduler: process_generator.c scheduler.c headers.h
	$(CC) $(CFLAGS) -o priority_scheduler process_generator.c scheduler.c

clean:
	rm -f priority_scheduler