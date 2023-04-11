CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS =

all : sys_monitoring_tool user_stats cpu_stats memory_stats

sys_monitoring_tool : sys_monitoring_tool.c
	gcc -Wall -g -o sys_monitoring_tool sys_monitoring_tool.c

user_stats : user_stats.c
	gcc -Wall -g -o user_stats user_stats.c

memory_stats : memory_stats.c
	gcc -Wall -g -o memory_stats memory_stats.c

cpu_stats : cpu_stats.c
	gcc -Wall -g -o cpu_stats cpu_stats.c

clean:
	rm -f sys_monitoring_tool user_stats cpu_stats memory_stats *.o
