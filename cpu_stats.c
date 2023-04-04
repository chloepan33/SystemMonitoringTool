

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Displaying the number of cores of current system.
 *
 * @return void
 */
void ShowCore() {
  printf("----------------------------\n");
  int core_num = sysconf(_SC_NPROCESSORS_ONLN);
  if (core_num == -1) {
    perror("sysconf");
    exit(1);
  }
  printf("Number of cores: %d\n", core_num);
}

/**
 * @brief Displaying the utilization percentage of CPU by reading file
 * /proc/stat
 *
 * @param period indicate how frequently to sample in seconds
 *
 * @return the utilization percentage of CPU
 */
double ShowCpu(int period) {
  unsigned long long pre[4];
  unsigned long long aft[4];
  unsigned long long diff[4];
  char cpu[10];
  FILE *fp;
  fp = fopen("/proc/stat", "r"); // open cpu information file
  fscanf(fp, "%s %llu %llu %llu %llu", cpu, &pre[0], &pre[1], &pre[2],
         &pre[3]); // read initial cpu values
  sleep(period);   // wait for period of time
  rewind(fp);      // refresh the file
  fscanf(fp, "%s %llu %llu %llu %llu", cpu, &aft[0], &aft[1], &aft[2],
         &aft[3]); // read current cpu values
  fclose(fp);
  for (int i = 0; i < 4; i++) {
    diff[i] = aft[i] - pre[i]; // caculate differences
  }
  unsigned long long percent = diff[0] + diff[1] + diff[2];
  unsigned long long total = percent + diff[3];
  printf("CPU usage: %.2f%%\n",
         (double)percent / (double)total * 100); // caculate cpu usage
  return (double)percent / (double)total * 100;
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
void CpuGraph(double cpu) {
  printf("\t");
  for (int i = 0; i < (int)cpu; i++) {
    printf("|");
  }
  printf("%.2f\n", cpu * 0.01);
}

int main(int argc, char *argv[]) {
  const char *special_string = "##SPECIAL_STRING##";
  setbuf(stdout, NULL); // disable buff
  int sample_size = 10;
  int period = 1;
  int graphic_state = 0;
  double cpu = 0;

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
    ShowCore();
    cpu = ShowCpu(period);
    if (graphic_state == 1) {
      CpuGraph(cpu);
    }
    printf("%s\n", special_string);
  }
}
