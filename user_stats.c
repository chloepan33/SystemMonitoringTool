
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmp.h>

/**
 * @brief Displaying information of current login users, including
 *    username,
 *    device name,
 *    host name for remote login.
 */

int main(int argc, char *argv[]) {
  const char *special_string = "##SPECIAL_STRING##";
  setbuf(stdout, NULL); // disable buff
  int sample_size = 10;
  int period = 1;
  int graphic_state = 0;

  if (argc > 1) {
    // scan entered arguments
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
