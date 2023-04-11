#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief handler for control c signal
 *
 * ask user if they really wannt quit the program
 *  
 * @param sig
 */
void ctrlc_handler(int sig)
{
  fprintf(stderr, "Do you really want to quit? [y/n] ");
  char c;        // a char to store user input
  c = getchar(); // read user input (y or n)
  if (c == 'y' || c == 'Y')
  {
    exit(0); // if user does what to quit, terminate
  }
  else
  {            // else reset the ctrl-c to this handler
    getchar(); // get a newline character

    // reset the ctrl-c to this handler function
    if (signal(sig, ctrlc_handler) == SIG_ERR)
    {
      perror("signal");
      exit(1);
    }
    printf("\x1b[1F"); // move up one line
    printf("\033[2K"); // erase the line
  }
}

/**
 * @brief handler for control z signal
 * 
 * it will ignore the signal and do nothing
 * 
 * @param sig 
 */

void ctrlz_handler(int sig)
{
  // reset the ctrl-z to this handler function
  if (signal(sig, ctrlz_handler) == SIG_ERR)
  {
    perror("signal");
    exit(1);
  }
  printf("\033[2D"); // erase "^Z"
}


/**
 * @brief Set the signals object for control z and control c
 * and perform error checking
 * 
 */
void set_signals()
{
  if (signal(SIGINT, ctrlc_handler) == SIG_ERR ||
      signal(SIGTSTP, ctrlz_handler) == SIG_ERR)
  {
    perror("signal");
    exit(1);
  }
}

/**
 * @brief save current cursor position
 * 
 */
void saveCursorPosition(void)
{
  // printf("\x1B[s"); // Send ANSI escape sequence to save cursor position.
  printf("\0337");
}

/**
 * @brief restore the most recently saved cursor position
 * 
 */
void restoreCursorPosition(void)
{
  // printf("\x1B[u"); // Send ANSI escape sequence to restore cursor
  // position.
  printf("\0338");
}


/**
 * @brief read output writte in the given result file
 * 
 * @param read_file 
 */
void read_output(FILE *read_file)
{
  char result[512];
  const char *special_string = "##SPECIAL_STRING##";
  while (fgets(result, 512, read_file) != NULL)
  {
    // read from pip line by line
    if (strncmp(result, special_string, strlen(special_string)) == 0)
    {
      // if read the special string indicating one iteration done
      // break the loop
      break;
    }
    else
    {
      printf("%s", result);
    }
  }
}

/**
 * @brief Displaying memory used by the current program in unit of kilobytes
 *
 * If fail to get memory usage, show error message
 *
 * @return void
 */

void ShowMemoryUsage()
{
  struct rusage r_usage; // get current program memory usage
  if (getrusage(RUSAGE_SELF, &r_usage) < 0)
  {
    perror("getrusage"); // if fail, show error message
    exit(1);
  }
  else
  {
    // else print out memory usage in unit of kilobytes
    printf("Memory usage: %ld kilobytes\n", r_usage.ru_maxrss);
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
 * If fail to get system information, show error message
 *
 * @return void
 */
void ShowSystemInfo()
{
  printf("----------------------------\n");
  struct utsname uts; // get system information
  if (uname(&uts) < 0)
  {
    perror("SystemInfo error"); // If fail, show error message
  }
  else
  {
    // else if success, print out detailed informations
    printf("### System Information ### \n");
    printf("System Name:  %s\n", uts.sysname);
    printf("Machine Name:  %s\n", uts.nodename);
    printf("Version:  %s\n", uts.version);
    printf("Release:  %s\n", uts.release);
    printf("Architecture: %s\n", uts.machine);
  }
  printf("----------------------------\n");
}

/**
 * @brief create child process for running independent c program
 * 
 * @param fd reserved space for pipe fds
 * @param file indicate which c program to run
 * @param argv arguments passed to the child c program
 */
void RunStats(int *fd, char *file, char *argv[])
{
  // creating pipe
  if (pipe(fd) < 0)
  {
    perror("pipe()");
    exit(1);
  }

// creating child process
  int id = fork();
  if (id < 0)
  {
    perror("fork()");
    exit(1);
  }

  if (id == 0)
  {
    // child process 1, reponsible for memory usage
    // close all read fds and the write fds that wont be used

    close(fd[0]);

    // Redirect standard output to write end of pipe
    if (dup2(fd[1], STDOUT_FILENO) == -1)
    {
      perror("dup2");
      exit(1);
    }
    // Run "memory_stats" command
    execvp(file, argv);
    // otherwise execlp fail, display erro message
    perror("execlp");
    exit(1);
  }
}


/**
 * @brief run all three child programs and read outputs in main process
 * 
 * @param sample_size 
 * @param sequential_state if 1, then print output in sequential form 
 * @param system_state if 1, then dont print user info
 * @param mem_argv arguments array passed to memory child program
 * @param user_argv arguments array passed to user child program
 * @param cpu_argv arguments array passed to cpu child program
 */
void ShowDefault(int sample_size, int sequential_state, int system_state,
                 char *mem_argv[], char *user_argv[], char *cpu_argv[])
{
  // initial fds
  int mem_fd[2];
  int user_fd[2];
  int cpu_fd[2];
  char *mem = "./memory_stats";
  char *user = "./user_stats";
  char *cpu = "./cpu_stats";

  RunStats(mem_fd, mem, mem_argv);
  RunStats(user_fd, user, user_argv);
  RunStats(cpu_fd, cpu, cpu_argv);

  // else we are in parent process
  // close all write fds
  close(cpu_fd[1]);
  close(mem_fd[1]);
  close(user_fd[1]);

  // struct file for each of read fds
  // therefore we can use fgets later
  FILE *mem_file = fdopen(mem_fd[0], "r");
  FILE *user_file = fdopen(user_fd[0], "r");
  FILE *cpu_file = fdopen(cpu_fd[0], "r");
  if ((mem_file == NULL) || (user_file == NULL) || (cpu_file == NULL))
  {
    perror("fdopen");
    exit(1);
  }

  // to print each iteration in sequential form
  if (sequential_state == 1)
  {
    for (int i = 0; i < sample_size; i++)
    {
      printf(">>> iteration %d\n", i + 1); // indicate which iteration 
      ShowMemoryUsage();
      printf("----------------------------\n");
      printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot) \n");
      for (int m = 0; m < i; m++)
      {
        printf("\n");
      }
      read_output(mem_file);
      for (int c = 1; c < sample_size - i; c++)
      {
        printf("\n");
      }
      if (system_state == 0)
      {
        read_output(user_file);
      }
      read_output(cpu_file);
      printf("----------------------------\n");
    }
    ShowSystemInfo();
    return;
  }

  // else print out result in default form

  // print memory usage
  ShowMemoryUsage();

  saveCursorPosition();
  for (int i = 0; i < sample_size; i++)
  {
    restoreCursorPosition();
    read_output(mem_file);
    saveCursorPosition();

    for (int j = 0; j < sample_size - 1 - i; j++)
    {
      printf("\n");
    }
    if (system_state == 0)
    {
      read_output(user_file);
    }
    read_output(cpu_file);
  }
  ShowSystemInfo();
}

int main(int argc, char *argv[])
{

  set_signals(); // set signal

  // initialize default argvs for child process
  char *mem_argv[5] = {"memory_stats", "--samples=10", "--tdelay=1", NULL,
                       NULL};
  char *cpu_argv[5] = {"cpu_stats", "--samples=10", "--tdelay=1", NULL, NULL};
  char *user_argv[5] = {"user_stats", "--samples=10", "--tdelay=1", NULL, NULL};

  // set default value of sample size and sampled frequency
  int sample_size = 10;
  int period = 1;

  int count_int = 0; // count how many integers user has inputed
  int tem_int = 0;   // store input integer temporarlity

  // to indicate whether each flag is activate or not
  int system_state = 0;
  int user_state = 0;
  int graphic_state = 0;
  int sequential_state = 0;

  if (argc == 1) // if user enter 0 command line arguments
  {
    printf("----------------------------\n");
    printf("Nbr of samples: %d -- every %d secs\n", sample_size, period);
    ShowDefault(sample_size, sequential_state, system_state, mem_argv,
                user_argv, cpu_argv);
    return 0;
  }
  else
  {
    // scan all entered arguments
    for (int i = 1; i < argc; i++)
    {
      // if valid arguments enterd, activiate corresponding state
      if (strcmp(argv[i], "--system") == 0)
      {
        system_state = 1;
      }
      else if (strcmp(argv[i], "--user") == 0)
      {
        user_state = 1;
      }
      else if (strcmp(argv[i], "--graphics") == 0)
      {
        graphic_state = 1;
        mem_argv[3] = argv[i];
        cpu_argv[3] = argv[i];
        user_argv[3] = argv[i];
      }
      else if (strcmp(argv[i], "--sequential") == 0)
      {
        sequential_state = 1;
      }
      // if sample size or frequency changed
      // update it and show message with current value
      else if (sscanf(argv[i], "--samples=%d", &sample_size) == 1 &&
               (sample_size > 0))
      {
        printf("The current sample size is %d\n", sample_size);
      }
      else if (sscanf(argv[i], "--tdelay=%d", &period) == 1 && (period > 0))
      {
        printf("The current sample frequency is %d sec\n", period);
      }
      // if integer entered
      else if (sscanf(argv[i], "%d", &tem_int) == 1 && (tem_int > 0))
      {
        if (count_int == 2)
        {
          printf("To many input integers!\n"); // if already have 2 integers,
                                               // display error message
          exit(0);
        }
        else if (count_int == 1)
        { // if only 1 integer enterd, store the
          // second one as new frequency
          period = tem_int;
          tem_int = 0;
          count_int = 2;
        }
        else if (count_int == 0)
        {
          // if it is the first integer, store the value as new sample size
          sample_size = tem_int;
          tem_int = 0;
          count_int = 1;
        }
      }
      else
      {
        // display error message for any other arguments
        printf("Invalid command line arguments\n");
        exit(0);
      }
    }
  }

  // show current sample size and frequency
  printf("----------------------------\n");
  printf("Nbr of samples: %d -- every %d secs\n", sample_size, period);
  char sample_size_string[20];
  char period_string[20];
  snprintf(sample_size_string, 20, "--samples=%d", sample_size);
  snprintf(period_string, 20, "--tdelay=%d", period);
  mem_argv[1] = sample_size_string;
  cpu_argv[1] = sample_size_string;
  user_argv[1] = sample_size_string;
  mem_argv[2] = period_string;
  cpu_argv[2] = period_string;
  user_argv[2] = period_string;
  if (user_state == 1) // if user state is avtivate
  {
    if (system_state == 1 || graphic_state == 1)
    {
      // any combination with other tate is considerd as invalid
      printf("Command combination invalid\n");
      exit(0);
    }
    else
    {
      // if only user_only state is activated
      // display user information according to period and sample size
      int user_fd[2];
      RunStats(user_fd, "./user_stats", user_argv);
      close(user_fd[1]);
      FILE *user_file = fdopen(user_fd[0], "r");
      if (user_file == NULL)
      {
        perror("fdopen");
        exit(1);
      }
      if (sequential_state == 1)
      {
        for (int i = 0; i < sample_size; i++)
        {
          printf(">>> iteration %d\n", i + 1);
          read_output(user_file);
        }
      }
      else
      {
        saveCursorPosition();
        for (int i = 0; i < sample_size; i++)
        {
          restoreCursorPosition();
          saveCursorPosition();
          read_output(user_file);
        }
      }

      return 0;
    }
  }
  ShowDefault(sample_size, sequential_state, system_state, mem_argv, user_argv,
              cpu_argv);
  return 0;
}