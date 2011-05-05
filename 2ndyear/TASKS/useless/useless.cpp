#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Returns false in case of failure, changes *result only on success.
bool ReadFileToString(const char *filename, /*OUT*/ std::string * const result) {
  assert(filename != NULL);
  assert(result != NULL);

  const int BUFFER_SIZE = 128;
  char buffer[BUFFER_SIZE + 1];  // "+ 1" is VERY important! See below.
  FILE *fp = fopen(filename, "r");
  if (!fp)
    return false;

  bool ret;
  std::string local_result;  // So we don't change *result in case of failure
  while (!feof(fp) && !ferror(fp)){
    int i = fread(buffer, sizeof(char), BUFFER_SIZE, fp);
    // Write '\0' after the bytes read so we can use buffer
    // as a C string.
    // VERY important! buffer size is "BUFFER_SIZE + 1",
    // otherwise you corrupt memory.
    buffer[i] = '\0';
    local_result += buffer;
  }
  if (!ferror(fp)) {
    *result = local_result;
    ret = true;
  } else {
    ret = false;
  }
  fclose(fp);

  return ret;
}

std::string GetFirstLineAndAdvance(std::string *str) {
  assert(str!= NULL);
  std::string ret;

  int next_eol = str->find_first_of('\n');
  if (next_eol == str->npos) {
    ret = *str;
    *str = "";
    return ret;
  }

  ret = str->substr(0, next_eol);
  *str = str->substr(next_eol + 1);
  return ret;
}

using namespace std;

struct DelayedCommand {
  int delay;
  string command;

  bool operator < (const DelayedCommand &other) const {
    return delay < other.delay;
  }
};

int running_commands = 0;

bool LaunchCommand(std::string command) {
  running_commands++;

  pid_t pid = fork();
  if (pid == 0) {
    // in child
    // TODO: argv, un-fix buffer
    char tmp[1024];
    strcpy(tmp, command.c_str());
    char * const args[] = {tmp, NULL};
    int ret = execvp(args[0], args);
    assert(ret == 0xDEADBEEF);
  }
}

void child_handler(int signum) {
  if (signum == SIGCHLD) {
    running_commands --;
  }
}

// {{{1
int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s <batch file>\n", argv[0]);
    return 1;
  }

  string batch;
  if (!ReadFileToString(argv[1], &batch)) {
    fprintf(stderr, "Can't read '%s' batch file\n", argv[1]);
    return 1;
  }

  vector<DelayedCommand> commands;
  while (batch != "") {
    string next_command = GetFirstLineAndAdvance(&batch);
    int space_idx = next_command.find_first_of(' ');
    if (space_idx == next_command.npos) {
      fprintf(stderr, "Malformed batch line '%s'\n", next_command.c_str());
      return 1;
    }

    DelayedCommand cmd;
    cmd.delay = atoi(next_command.substr(0, space_idx).c_str());
    cmd.command = next_command.substr(space_idx + 1);
    commands.push_back(cmd);
  }

  std::sort(commands.begin(), commands.end());

  signal(SIGCHLD, child_handler);

  for (int i = 0; i < commands.size(); i++) {
    // TODO: usleep
    LaunchCommand(commands[i].command);
  }

  while (running_commands > 0)
    ::wait(NULL);

  return 0;
}
// }}}
