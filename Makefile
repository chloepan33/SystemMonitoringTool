CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS =

all : test user_stats cpu_stats memory_stats

test : test.c
	gcc -Wall -g -o test test.c

user_stats : user_stats.c
	gcc -Wall -g -o user_stats user_stats.c

memory_stats : memory_stats.c
	gcc -Wall -g -o memory_stats memory_stats.c

cpu_stats : cpu_stats.c
	gcc -Wall -g -o cpu_stats cpu_stats.c

clean:
	rm -f test user_stats cpu_stats memory_stats *.o
