#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <utmp.h>

/**
 * @brief Displaying memory used by the current program in unit of kilobytes
 *
 * If fail to get memory usage, show error message
 *
 * @return void
 */

void ShowMemoryUsage() {
  struct rusage r_usage; // get current program memory usage
  if (getrusage(RUSAGE_SELF, &r_usage) < 0) {
    perror("MemoryUsage error"); // if fail, show error message
  } else {
    // else print out memory usage in unit of kilobytes
    printf("Memory usage: %ld kilobytes\n", r_usage.ru_maxrss);
  }
}

/**
 * @brief Displaying memory variation represented by graph
 *
 * @param pre previous memory size
 * @param post current memory size
 *
 * @return void
 */

void MemroyGraph(double pre, double post) {
  // caculate difference between previous memory and current memory
  double diff = post - pre;
  printf("|");   // start symbol for memory graph
  if (diff >= 0) // if memory increase
  {
    // if the increase amount is less than 0.01GB or it is the first time
    // reading memory
    if (diff < 0.01 || pre < 0) {
      printf("o"); // use "o" to indicate
    } else {
      for (int i = 0; i < (int)diff * 10;
           i++) // if memroy increase mroe than 0.01GB
      {
        printf("#"); // use "#" to represent variation propotionally
      }
      printf("*"); // symble indicate end of graph
    }
  } else // else if memroy decrease
  {
    // if the decrease amount is less than 0.01GB
    if (diff >= -0.01) {
      printf("@"); // use "@" to indicate
    } else {
      for (int i = 0; i < (int)-diff * 10; i++) {
        printf(":"); // use ":" to represent variation propotionally
      }
      printf("@"); // symble indicate end of graph
    }
    diff = -diff; // change difference to its absolute value
  }
  printf(" %.2f (%.2f)\n", diff, post);
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
double ShowMemory(double pre, int graph_state) {
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
  printf("%.2f GB / %.2f GB  -- %.2f GB / %.2f GB", phys_used * 1e-9,
         total_phys * 1e-9, virtual_used * 1e-9, total_virtual * 1e-9);
  if (graph_state == 0) {
    printf("\n");
  } else {
    MemroyGraph(pre * 1e-9, phys_used * 1e-9);
  }
  return (double)phys_used;
}

int main(int argc, char *argv[]) {
  const char *special_string = "##SPECIAL_STRING##";
  setbuf(stdout, NULL); // disable buff
  int sample_size = 10;
  int period = 1;
  int graphic_state = 0;
  double pre = 0;

  // set the ctrl-c signal to defalut and ctrl-z to be ignored
  if (signal(SIGINT, SIG_IGN) == SIG_ERR ||
      signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(1);
  }

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (sscanf(argv[i], "--samples=%d", &sample_size) == 1 &&
          (sample_size > 0)) {
        continue;
      } else if (sscanf(argv[i], "--tdelay=%d", &period) == 1 && (period > 0)) {
        continue;
      } else if (strcmp(argv[i], "--graphics") == 0) {
        graphic_state = 1;
      }
    }
  }

  for (int i = 0; i < sample_size; i++) {
    pre = ShowMemory(pre, graphic_state);
    printf("%s\n", special_string);
    sleep(period);
  }
}
