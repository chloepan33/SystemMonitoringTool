
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmp.h>

/**
 * @brief main function for getting user info
 *
 * Sample once every 1 sec and sample total of 10 times in default
 * Able to take command line argument to print each itertion in either
 * sequential or refreshing form.
 *
 * @param argc
 * @param argv
 * @return int
 */

int main(int argc, char *argv[]) {
  const char *special_string =
      "##SPECIAL_STRING##"; // indicate one iteration has done
  setbuf(stdout, NULL);     // disable buff
  int sample_size = 10;
  int period = 1;

  // set the ctrl-c signal and ctrl-z to be ignored
  if (signal(SIGINT, SIG_IGN) == SIG_ERR ||
      signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(1);
  }

  if (argc > 1) {
    // scan entered arguments
    for (int i = 1; i < argc; i++) {
      if (sscanf(argv[i], "--samples=%d", &sample_size) == 1 &&
          (sample_size > 0)) {
        continue;
      } else if (sscanf(argv[i], "--tdelay=%d", &period) == 1 && (period > 0)) {
        continue;
      }
    }
  }
  for (int i = 0; i < sample_size; i++) {
    printf("----------------------------\n");
    printf("### Sessions/users ### \n");
    struct utmp *data;
    setutent();
    data = getutent(); // get user data
    if (data == NULL) {
      perror("getutent"); // if fail to get user info
      exit(1);
    }
    while (data != NULL) {
      // only print out normal user process
      if (data->ut_type == USER_PROCESS) {
        // print out user information
        printf("%s %s %s\n", data->ut_name, data->ut_line, data->ut_host);
      }
      data = getutent(); // next user
    }
    endutent();
    printf("%s\n", special_string);
    sleep(period);
  }

  exit(0);
}
