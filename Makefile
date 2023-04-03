CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99 -O2
LDFLAGS=

.PHONY: all clean

all: test user_stats memory_stats cpu_stats

test: test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

user_stats: user_stats.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

memory_stats: memory_stats.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

cpu_stats: cpu_stats.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f test user_stats memory_stats cpu_stats
