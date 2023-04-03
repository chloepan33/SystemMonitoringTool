/**
 * @file stats_functions.c
 * @author Zhiyu Pan (zypan03@gmail.com)
 * @brief A simple command line program written in C that provides information
 * about basic system information, CPU utilization, memory utilization, and
 * users’ information.
 * @date 2023-02-05
 *
 */
#include <sys/wait.h>
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <utmp.h>

// /**
//  * @brief Moves the cursor up.
//  *
//  * @param positions Number of lines to move up
//  * @return void
//  */
// void moveUp(int positions) { printf("\x1b[%dF", positions); }

/**
 * @brief Moves the cursor down.
 *
 * @param positions Number of lines to move down
 * @return void
 */
void moveDown(int positions) { printf("\x1b[%dE", positions); }

void saveCursorPosition(void) {
  printf("\033[s"); // Send ANSI escape sequence to save cursor position.
}

void restoreCursorPosition(void) {
  printf("\033[u"); // Send ANSI escape sequence to restore cursor position.
}

void read_output(int read_fd) {
  char result[1024];
  char *done = "done";
  while (read(read_fd, result, 1024) > 0) {
    if (strcmp(result, done) == 0) {
      break;
    } else {
      printf("%s", result);
    }
  }
}

/**
 * @brief Displaying the system information, including (in the order of)
 *    System Name,
 *    Machine Name,
 *    Version,
 *    Release,
 *    Architecture.
 *
 * @param write_fd Fds for writing
 *
 * @return void
 */
void ShowSystemInfo(int write_fd) {
  char result[1024];
  struct utsname uts; // get system information
  if (uname(&uts) < 0) {
    perror("SystemInfo error"); // If fail, show error message
  } else {
    snprintf(result, 1024,
             "----------------------------\nSystem Name:  %s\nMachine Name:  "
             "%s\nVersion:  %s\nRelease:  "
             "%s\nArchitecture: %s\n----------------------------\n",
             uts.sysname, uts.nodename, uts.version, uts.release, uts.machine);
    write(write_fd, result, sizeof(result));
  }
  close(write_fd);
}

/**
 * @brief Displaying memory used by the current program in unit of kilobytes
 *
 * If fail to get memory usage, show error message
 *
 * @return void
 */
void ShowMemoryUsage(int write_fd) {
  char result[1024];
  struct rusage r_usage; // get current program memory usage
  if (getrusage(RUSAGE_SELF, &r_usage) < 0) {
    perror("MemoryUsage error"); // if fail, show error message
  } else {
    // else print out memory usage in unit of kilobytes
    snprintf(result, 1024, "Memory usage: %ld kilobytes\n", r_usage.ru_maxrss);
    write(write_fd, result, sizeof(result));
  }
  close(write_fd);
}

/**
 * @brief Displaying memory variation represented by graph
 *
 * @param pre previous memory size
 * @param post current memory size
 *
 * @return void
 */

void MemroyGraph(double pre, double post, int write_fd) {
  char result[1024];
  char *done = "done";
  // caculate difference between previous memory and current memory
  double diff = post - pre;
  snprintf(result, 1024, "|"); // start symbol for memory graph
  write(write_fd, result, 1024);
  if (diff >= 0) // if memory increase
  {
    // if the increase amount is less than 0.01GB or it is the first time
    // reading memory
    if (diff < 0.01 || pre < 0) {
      snprintf(result, 1024, "o"); // use "o" to indicate
      write(write_fd, result, 1024);
    } else {
      // if memroy increase mroe than 0.01GB
      for (int i = 0; i < (int)diff * 10; i++) {
        // use "#" to represent variation propotionally
        snprintf(result, 1024, "#");
        write(write_fd, result, 1024);
      }
      // symble indicate end of graph
      snprintf(result, 1024, "*");
      write(write_fd, result, 1024);
    }
  } else // else if memroy decrease
  {
    // if the decrease amount is less than 0.01GB
    if (diff >= -0.01) {
      // use "@" to indicate
      snprintf(result, 1024, "@");
      write(write_fd, result, 1024);
    } else {
      for (int i = 0; i < (int)-diff * 10; i++) {
        // use ":" to represent variation propotionally
        snprintf(result, 1024, ":");
        write(write_fd, result, 1024);
      }
      // symble indicate end of graph
      snprintf(result, 1024, "@");
      write(write_fd, result, 1024);
    }
    diff = -diff; // change difference to its absolute value
  }
  snprintf(result, 1024, " %.2f (%.2f)\n", diff, post);
  write(write_fd, result, 1024);
  write(write_fd, done, strlen(done) + 1);
}

/**
 * @brief Displaying memory information, in unit of GB, including
 *    total physical memory,
 *    used physical memory,
 *    total virtual memory,
 *    used virtual memory.
 *
 *    Memories are calculated by reading file /proc/meminfo.
 *    total physical memory = totalram
 *    used physical memory = (totalram - freeram)  - (bufferram + cachedram +
 * SReclaimable) total virtual memory = totalram + total swap used virtual
 * memory = used physical memory + totalswap - freeswap
 *
 * @param pre value of previous used memory size
 * @param graph_state to indicate whether or not to show graphics
 * @return the used physical memory
 *
 */
void ShowMemory(int graph_state, int period, int sample_size, int write_fd) {
  char *done = "done";
  char result[1024];
  double post;
  double pre = 0;
  for (int i = 0; i < sample_size; i++) {
    FILE *meminfo = fopen("/proc/meminfo", "r"); // open memory information file
    char line[200];
    long totalram, freeram, bufferram, cachedram, totalswap, freeswap, sre;
    long phys_used, total_phys, virtual_used, total_virtual;

    // scan file to get memory information needed
    // and convert scaned value to unit of byte
    while (fgets(line, sizeof(line), meminfo)) {
      if (sscanf(line, "MemTotal: %ld kB", &totalram) == 1) {
        totalram *= 1024;
      } else if (sscanf(line, "MemFree: %ld kB", &freeram) == 1) {
        freeram *= 1024;
      } else if (sscanf(line, "Buffers: %ld kB", &bufferram) == 1) {
        bufferram *= 1024;
      } else if (sscanf(line, "Cached: %ld kB", &cachedram) == 1) {
        cachedram *= 1024;
      } else if (sscanf(line, "SwapTotal: %ld kB", &totalswap) == 1) {
        totalswap *= 1024;
      } else if (sscanf(line, "SwapFree: %ld kB", &freeswap) == 1) {
        freeswap *= 1024;
      } else if (sscanf(line, "SReclaimable: %ld kB", &sre) == 1) {
        sre *= 1024;
        fclose(meminfo);
      }
    }
    total_phys = totalram;
    phys_used = (totalram - freeram) - (bufferram + cachedram + sre);
    total_virtual = totalram + totalswap;
    virtual_used = phys_used + totalswap - freeswap;

    snprintf(result, 1024, "%.2f GB / %.2f GB  -- %.2f GB / %.2f GB",
             phys_used * 1e-9, total_phys * 1e-9, virtual_used * 1e-9,
             total_virtual * 1e-9);
    write(write_fd, result, 1024);

    if (graph_state == 0) {

      snprintf(result, 1024, "\n");
      write(write_fd, result, 1024);
      write(write_fd, done, strlen(done) + 1);
    } else {
      post = phys_used * 1e-9;
      MemroyGraph(pre, post, write_fd);
      pre = post;
    }

    sleep(period);
  }
  close(write_fd);
}

/**
 * @brief Displaying information of current login users, including
 *    username,
 *    device name,
 *    host name for remote login.
 *
 * @return void
 */
void ShowUser(int period, int sample_size, int write_fd) {
  char result[1024];
  char *done = "done";
  for (int i = 0; i < sample_size; i++) {
    char header[512] =
        "----------------------------\n### Sessions/users ### \n";
    if (write(write_fd, header, sizeof(header)) == -1) {
      perror("write()");
      exit(0);
    }
    struct utmp *data;
    setutent();
    data = getutent(); // get user data
    if (data == NULL) {
      perror("getutent()");
      exit(1);
    }
    while (data != NULL) {
      if (data->ut_type == USER_PROCESS) {
        // print out user information
        snprintf(result, 1024, "%s %s %s\n", data->ut_name, data->ut_line,
                 data->ut_host);
        write(write_fd, result, sizeof(result));
      }
      data = getutent(); // next user
    }
    endutent();
    write(write_fd, done, strlen(done) + 1);
    sleep(period);
  }
  close(write_fd);
}

/**
 * @brief Displaying the number of cores of current system.
 *
 * @return void
 */
void ShowCore(int period, int sample_size, int write_fd) {
  char *done = "done";
  char result[1024];

  for (int i = 0; i < sample_size; i++) {
    snprintf(result, 1024, "----------------------------\n");
    write(write_fd, result, 1024);
    int core_num = sysconf(_SC_NPROCESSORS_ONLN);
    if (core_num == -1) {
      perror("sysconf()");
      exit(1);
    }
    snprintf(result, 1024, "Number of cores: %d\n", core_num);
    write(write_fd, result, 1024);
    write(write_fd, done, strlen(done) + 1);
    sleep(period);
  }
  close(write_fd);
}

/**
 * @brief Displaying cpu usage in graphic form
 *
 *    The number of symbol "|" is equals to number of percentage
 *
 * @param cpu used cpu percentage
 *
 * @return void
 */
void CpuGraph(double cpu, int write_fd) {
  char result[1024];
  snprintf(result, 1024, "\t");
  write(write_fd, result, 1024);
  for (int i = 0; i < (int)cpu - 99; i++) {
    snprintf(result, 1024, "|");
    write(write_fd, result, 1024);
  }
  snprintf(result, 1024, "%.2f\n", cpu * 0.01);
  write(write_fd, result, 1024);
}

/**
 * @brief Displaying the utilization percentage of CPU by reading file
 * /proc/stat
 *
 * @param period indicate how frequently to sample in seconds
 *
 * @return the utilization percentage of CPU
 */
void ShowCpu(int period, int sample_size, int graphic_state, int write_fd) {
  char *done = "done";
  char result[1024];
  unsigned long long pre[4];
  unsigned long long aft[4];
  unsigned long long diff[4];
  char cpu[10];
  FILE *fp;
  double cpuvalue;
  for (int i = 0; i < sample_size; i++) {
    fp = fopen("/proc/stat", "r"); // open cpu information file
    fscanf(fp, "%s %llu %llu %llu %llu", cpu, &pre[0], &pre[1], &pre[2],
           &pre[3]); // read initial cpu values

    sleep(period);
    rewind(fp); // refresh the file
    fscanf(fp, "%s %llu %llu %llu %llu", cpu, &aft[0], &aft[1], &aft[2],
           &aft[3]); // read current cpu values

    for (int i = 0; i < 4; i++) {
      diff[i] = aft[i] - pre[i]; // caculate differences
    }

    unsigned long long percent = diff[0] + diff[1] + diff[2];
    unsigned long long total = percent + diff[3];
    cpuvalue = (double)percent / (double)total * 100;
    snprintf(result, 1024, "CPU usage: %.2f%%\n",
             cpuvalue); // caculate cpu usage
    write(write_fd, result, sizeof(result));
    if (graphic_state == 1) {
      CpuGraph(cpuvalue, write_fd);
    }
    write(write_fd, done, strlen(done) + 1);
    fclose(fp);
  }

  close(write_fd);
}

void ShowDefault(int period, int sample_size) {

  printf("----------------------------\n");
  printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot) \n");
  // saveCursorPosition();

  int fds[3][2]; // one pipe for each of user, memory, cpu usage
  for (int i = 0; i < 3; i++) {
    if (pipe(fds[i]) < 0) {
      perror("pipe()");
      exit(1);
    }
  }

  int id1 = fork();
  if (id1 < 0) {
    perror("fork()");
    exit(1);
  }

  if (id1 == 0) {
    // child process 1, reponsible for memory usage
    // close all read fds and the write fds that wont be used
    for (int j = 0; j < 3; j++) {
      close(fds[j][0]);
      if (j != 0) {
        close(fds[j][1]);
      }
    }
    ShowMemory(1, period, sample_size, fds[0][1]);
    exit(0);
  } else {
    // if in the parent process
    int id2 = fork(); // fork again for user usage
    if (id2 < 0) {
      perror("fork()");
      exit(1);
    }
    if (id2 == 0) {
      // child process 2, reponsible for user
      // close all read fds and the write fds that wont be used
      for (int j = 0; j < 3; j++) {
        close(fds[j][0]);
        if (j != 1) {
          close(fds[j][1]);
        }
      }
      // ShowUser(period, sample_size, fds[1][1]);
      ShowCore(period, sample_size, fds[1][1]);
      exit(0);
    } else {
      // if it is the parent process
      int id3 = fork();
      if (id3 < 0) {
        perror("fork()");
        exit(1);
      }
      if (id3 == 0) {
        // child process 3, resposible for cores
        // close all read fds and the write fds that wont be used
        for (int j = 0; j < 3; j++) {
          close(fds[j][0]);
          if (j != 2) {
            close(fds[j][1]);
          }
        }
        ShowCpu(period, sample_size, 0, fds[2][1]);
        exit(0);
      } else {
        // if it is in the parent process
        // close all write fds
        for (int j = 0; j < 3; j++) {
          close(fds[j][1]);
        }
        saveCursorPosition();
        for (int i = 0; i < sample_size; i++) {
          restoreCursorPosition();
          read_output(fds[0][0]);
          saveCursorPosition();
          for (int j = 0; j < sample_size - 1 - i; j++) {
            printf("\n");
          }
          read_output(fds[1][0]);
          read_output(fds[2][0]);
        }

        close(fds[0][0]);
        close(fds[1][0]);
        close(fds[2][0]);
      }
    }
  }
}

// /**
//  * @brief Displaying system information including memory usage and cpu
//  usage in
//  * sequantial form
//  *
//  * @param sample_size indicate how many times the statistics are going to
//  be
//  * collected
//  * @param period indicate how frequently to sample in seconds
//  * @param graphic_state indicate whether or not to show graphics
//  *
//  * @return void
//  */
// void ShowSequentials(int sample_size, int period, int graph_state) {
//   double pre = -1;
//   for (int i = 0; i < sample_size; i++) {
//     printf(">>> iteration %d\n", i + 1);
//     ShowMemoryUsage();
//     printf("----------------------------\n");
//     printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot) \n");
//     for (int m = 0; m < i; m++) {
//       printf("\n");
//     }
//     pre = ShowMemory(pre, graph_state);
//     for (int c = 1; c < sample_size - i; c++) {
//       printf("\n");
//     }
//     printf("----------------------------\n");
//     double cpu = ShowCpu(period);
//     if (graph_state == 1) {
//       CpuGraph(cpu);
//     }
//     printf("----------------------------\n");
//   }
// }
