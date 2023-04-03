#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void saveCursorPosition(void) {
  printf("\033[s"); // Send ANSI escape sequence to save cursor position.
}

void restoreCursorPosition(void) {
  printf("\033[u"); // Send ANSI escape sequence to restore cursor position.
}

void read_output(FILE *read_file) {
  char result[512];
  const char *special_string = "##SPECIAL_STRING##";
  while (fgets(result, 512, read_file) != NULL) {
    // read from pip line by line
    if (strncmp(result, special_string, strlen(special_string)) == 0) {
      break;
    } else {
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

void ShowMemoryUsage() {
  struct rusage r_usage; // get current program memory usage
  if (getrusage(RUSAGE_SELF, &r_usage) < 0) {
    perror("getrusage"); // if fail, show error message
    exit(1);
  } else {
    // else print out memory usage in unit of kilobytes
    printf("Memory usage: %ld kilobytes\n", r_usage.ru_maxrss);
  }
}

void ShowDefault() {
  int fds[3][2]; // one pipe for each of user, memory, cpu usage
  for (int i = 0; i < 3; i++) {
    if (pipe(fds[i]) < 0) {
      perror("pipe()");
      exit(1);
    }
  }

  int mem_id = fork();
  if (mem_id < 0) {
    perror("fork()");
    exit(1);
  }

  if (mem_id == 0) {
    // child process 1, reponsible for memory usage
    // close all read fds and the write fds that wont be used
    for (int j = 0; j < 3; j++) {
      close(fds[j][0]);
      if (j != 0) {
        close(fds[j][1]);
      }
    }

    // Redirect standard output to write end of pipe
    if (dup2(fds[0][1], STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(1);
    }
    // Run "memory_stats" command
    execlp("./memory_stats", "memory_stats", NULL);
    // otherwise execlp fail, display erro message
    perror("execlp");
    exit(1);
  }

  int user_id = fork();
  if (user_id < 0) {
    perror("fork()");
    exit(1);
  }

  if (user_id == 0) {
    // child process 2, reponsible for user information
    // close all read fds and the write fds that wont be used
    for (int j = 0; j < 3; j++) {
      close(fds[j][0]);
      if (j != 1) {
        close(fds[j][1]);
      }
    }
    // Redirect standard output to write end of pipe
    if (dup2(fds[1][1], STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(1);
    }
    // Run "user_stats" command
    execlp("./user_stats", "user_stats", NULL);
    // otherwise execlp fail, display erro message
    perror("execlp");
    exit(1);
  }

  int cpu_id = fork();
  if (cpu_id < 0) {
    perror("fork()");
    exit(1);
  }

  if (cpu_id == 0) {
    // child process 2, reponsible for user information
    // close all read fds and the write fds that wont be used
    for (int j = 0; j < 3; j++) {
      close(fds[j][0]);
      if (j != 2) {
        close(fds[j][1]);
      }
    }
    // Redirect standard output to write end of pipe
    if (dup2(fds[2][1], STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(1);
    }
    // Run "cpu_stats" command
    execlp("./cpu_stats", "cpu_stats", NULL);
    // otherwise execlp fail, display erro message
    perror("execlp");
    exit(1);
  }

  // else we are in parent process
  // close all write fds
  for (int j = 0; j < 3; j++) {
    close(fds[j][1]);
  }

  FILE *mem_file = fdopen(fds[0][0], "r");
  FILE *user_file = fdopen(fds[1][0], "r");
  FILE *cpu_file = fdopen(fds[2][0], "r");
  if ((mem_file == NULL) || (user_file == NULL) || (cpu_file == NULL)) {
    perror("fdopen");
    exit(1);
  }

  // print out header
  printf("Nbr of samples: %d -- every %d secs\n", 10, 1);
  printf("----------------------------\n");
  // print memory usage
  ShowMemoryUsage();

  saveCursorPosition();
  for (int i = 0; i < 10; i++) {
    restoreCursorPosition();
    read_output(mem_file);
    saveCursorPosition();
    for (int j = 0; j < 9 - i; j++) {
      printf("\n");
    }
    read_output(user_file);
    read_output(cpu_file);
  }
}

int main(int argc, char *argv[]) {

  ShowDefault();
  // const char *special_string = "##SPECIAL_STRING##";
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
  //   // show defalut information
  // }

  // int fd[2];
  // pid_t pid;
  // char result[512];
  // int i = 0;

  // /* Create pipe */
  // if (pipe(fd) == -1) {
  //   perror("pipe");
  //   return 1;
  // }

  // /* Fork first child */
  // pid = fork();
  // if (pid == -1) {
  //   perror("fork");
  //   return 1;
  // } else if (pid == 0) {
  //   /* write to pipe */
  //   close(fd[0]); /* Close unused read end of pipe */

  //   /* Redirect standard output to write end of pipe */
  //   if (dup2(fd[1], STDOUT_FILENO) == -1) {
  //     perror("dup2");
  //     exit(1);
  //   }
  //   execlp("./memory_stats", "memory_stats", NULL); /* Run "user_stats"
  //   command */ perror("execlp"); exit(1);
  // }

  // /* Parent process - wait for children to complete */
  // close(fd[1]); /* Close write end of pipe */

  // FILE *pipe_file = fdopen(fd[0], "r");
  // if (pipe_file == NULL) {
  //   perror("fdopen");
  //   exit(1);
  // }
  // while (fgets(result, 512, pipe_file) != NULL) {
  //   // read from pip line by line
  //   if (strncmp(result, special_string, strlen(special_string)) == 0) {
  //     printf("iteration %d is done\n", i++);
  //   } else {
  //     printf("%s", result);
  //   }
  // }

  // /* Wait for child process to complete */
  // wait(NULL);

  // return 0;
}
