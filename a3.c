/**
 * @file system_monitoring_tool.c
 * @author Zhiyu Pan (zypan03@gmail.com)
 * @brief A simple command line program written in C that provides information
 * about basic system information, CPU utilization, memory utilization, and
 * users’ information.
 * @date 2023-02-05
 *
 */

#include "stats_functions.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utmp.h>

int main(int argc, char *argv[]) {

  // ShowDefault(2, 10);
  // return 0;
  // char *done = "done";
  // int i = 1;
  // int fds[2];
  // pipe(fds);

  // int id = fork();
  // if (id == 0) {
  //   close(fds[0]);
  //   // ShowSystemInfo(fds[1]);
  //   // ShowMemoryUsage(fds[1]);
  //   // ShowUser(fds[1]);
  //   // ShowCore(1,10,fds[1]);
  //   ShowCpu(1, 10, 0, fds[1]);
  //   // ShowMemory(1, 1, 10, fds[1]);
  //   // close(fds[1]);
  // } else {
  //   close(fds[1]);
  //   char result[1024];
  //   while (read(fds[0], result, 1024) > 0) {
  //     if (strcmp(result, done) == 0) {
  //       printf("this is iteration %d done\n", i);
  //       i++;
  //     } else {
  //       printf("%s", result);
  //     }
  //   }
  //   close(fds[0]);
  //   wait(NULL);
  // }

  // // set default value of sample size and sampled frequency
  // int sample_size = 10;
  // int period = 1;

  // int count_int = 0; // count how many integers user has inputed
  // int tem_int = 0;   // store input integer temporarlity

  // // to indicate whether each flag is activate or not
  // int system_state = 0;
  // int user_state = 0;
  // int graphic_state = 0;
  // int sequential_state = 0;

  // if (argc == 1) // if user enter 0 command line arguments
  // {
  //    // show defalut information
  //    printf("Nbr of samples: %d -- every %d secs\n", sample_size, period);
  //    ShowSystem(sample_size, period, graphic_state);
  //    ShowUser();
  //    ShowSystemInfo();
  // }
  // else
  // {
  //    // scan all entered arguments
  //    for (int i = 1; i < argc; i++)
  //    {
  //       // if valid arguments enterd, activiate corresponding state
  //       if (strcmp(argv[i], "--system") == 0)
  //       {
  //          system_state = 1;
  //       }
  //       else if (strcmp(argv[i], "--user") == 0)
  //       {
  //          user_state = 1;
  //       }
  //       else if (strcmp(argv[i], "--graphics") == 0)
  //       {
  //          graphic_state = 1;
  //       }
  //       else if (strcmp(argv[i], "--sequential") == 0)
  //       {
  //          sequential_state = 1;
  //       }
  //       // if sample size or frequency changed, update it and show message
  //       with current value else if (sscanf(argv[i], "--samples=%d",
  //       &sample_size) == 1 && (sample_size > 0))
  //       {
  //          printf("The current sample size is %d\n", sample_size);
  //       }
  //       else if (sscanf(argv[i], "--tdelay=%d", &period) == 1 && (period >
  //       0))
  //       {
  //          printf("The current sample frequency is %d sec\n", period);
  //       }
  //       // if integer entered
  //       else if (sscanf(argv[i], "%d", &tem_int) == 1 && (tem_int > 0))
  //       {
  //          if (count_int == 2)
  //          {
  //             printf("To many input integers!\n"); // if already have 2
  //             integers, display error message exit(0);
  //          }
  //          else if (count_int == 1)
  //          { // if only 1 integer enterd, store the second one as new
  //          frequency
  //             period = tem_int;
  //             tem_int = 0;
  //             count_int = 2;
  //          }
  //          else if (count_int == 0)
  //          { // if it is the first integer, store the value as new sample
  //          size
  //             sample_size = tem_int;
  //             tem_int = 0;
  //             count_int = 1;
  //          }
  //       }
  //       else
  //       {
  //          perror("Invalid command line arguments\n"); // display error
  //          message for any other arguments exit(0);
  //       }
  //    }
  //    // show current sample size and frequency
  //    printf("Nbr of samples: %d -- every %d secs\n", sample_size, period);
  //    if (user_state == 1) // if user state is avtivate
  //    {
  //       if (system_state == 1 || sequential_state == 1 || graphic_state == 1)
  //       {
  //          perror("Command combination invalid\n"); // any combination with
  //          other state is considerd as invalid exit(0);
  //       }
  //       else
  //       {
  //          ShowUser(); // if only user state is activated, only display user
  //          information
  //       }
  //    }
  //    else if (sequential_state == 1) // if only requird to display system
  //    information
  //    {
  //       ShowSequentials(sample_size, period, graphic_state); // display
  //       system info in sequential form
  //    }
  //    else
  //    {
  //       ShowSystem(sample_size, period, graphic_state); // other wise display
  //       system info in defalut form
  //    }

  //    if (system_state == 0) // if system_only state is not activate
  //    {
  //       ShowUser(); // show user information
  //    }

  //    ShowSystemInfo(); // show basic system information
  // }
}