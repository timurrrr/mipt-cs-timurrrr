#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <string>
#include <vector>

int main() {
  pid_t child_pid = fork();
  printf("==%d== After fork()\n", getpid());
  if (child_pid == 0) {
    // In child
    std::vector<std::string> args_vector;
    args_vector.push_back("echo");
    args_vector.push_back("1");
    args_vector.push_back("2");
    args_vector.push_back("3");

    // Turn args_vector (vector<string>) into char** to use in execvp call.
    // This is ugly but should be fine if wrapped in a separate function.
    char **args_array = new char*[args_vector.size() + 1];
    for (int i = 0; i < args_vector.size(); i++) {
      std::string cur_arg = args_vector[i].c_str();
      args_array[i] = new char[cur_arg.length() + 1];
      strcpy(args_array[i], cur_arg.c_str());
    }
    args_array[args_vector.size()] = NULL;

    execvp(args_array[0], args_array);
    // Shouldn't continue on success.
    // Not "delete"ing args_array is fine since we'll abort anyway.
    printf("Error running '%s'\n", args_array[0]);
    abort();
  } else {
    // In parent
    printf("==%d== In parent. child_pid = %d\n",
           getpid(), child_pid);
  }

  printf("==%d== Waiting for child to complete...\n", getpid());
  int status,
      ret = waitpid(child_pid, &status, 0);
  if (ret != child_pid) {
    // Error during waitpid
    printf("==%d== Hm, waitpid returned %d?..\n", getpid(), ret);
    return 1;
  }
  if (!WIFEXITED(status)) {
    // Exited abnormally
    printf("==%d== Hm, WIFEXITED == false!\n", getpid());
    return 1;
  }
  printf("==%d== DONE\n", getpid());
  return 0;
}
