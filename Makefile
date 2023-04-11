CC = gcc
CFLAGS = -Wall -Werror

all : sys_monitoring_tool user_stats cpu_stats memory_stats

sys_monitoring_tool : sys_monitoring_tool.o
	$(CC) -o $@ $<

user_stats : user_stats.o
	$(CC) -o $@ $<

memory_stats : memory_stats.o
	$(CC) -o $@ $<

cpu_stats : cpu_stats.o
	$(CC) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f sys_monitoring_tool user_stats cpu_stats memory_stats *.o
