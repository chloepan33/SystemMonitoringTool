#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  const char *special_string = "##SPECIAL_STRING##";
  int fd[2];
  pid_t pid;
  char result[512];
  int i = 0;

  /* Create pipe */
  if (pipe(fd) == -1) {
    perror("pipe");
    return 1;
  }

  /* Fork first child */
  pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  } else if (pid == 0) {
    /* write to pipe */
    close(fd[0]); /* Close unused read end of pipe */

    /* Redirect standard output to write end of pipe */
    if (dup2(fd[1], STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(1);
    }
    execlp("./memory_stats", "memory_stats", NULL); /* Run "user_stats" command */
    perror("execlp");
    exit(1);
  }

  /* Parent process - wait for children to complete */
  close(fd[1]); /* Close write end of pipe */

  FILE *pipe_file = fdopen(fd[0], "r");
  if (pipe_file == NULL) {
    perror("fdopen");
    exit(1);
  }
  while (fgets(result, 512, pipe_file) != NULL) {
    // // read from pip line by line
    // if (strncmp(result, special_string, strlen(special_string)) == 0) {
    //   printf("iteration %d is done\n", i++);
    // } else {
    //   printf("%s", result);
    // }
  }

  // /* Wait for child process to complete */
  // wait(NULL);

  // return 0;
}
