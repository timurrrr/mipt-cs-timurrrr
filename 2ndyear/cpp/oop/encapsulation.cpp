#include <stdio.h>
#include <string.h>

#include <string>

class Examiner {
 public:
  Examiner(const char *name) {
    name_ = name;
  }

  std::string name() const {
    return name_;
  }

  void set_correct_answer(const char *correct_answer) {
    correct_answer_ = correct_answer;
  }

  bool Answer(const char *answer) const {
    return strcmp(correct_answer_.c_str(),
                  answer) == 0;
  }

 private:
  std::string name_;
  std::string correct_answer_;
};

void StudentDuringTheExam(const Examiner &examiner) {
  printf("Hello, %s!\n", examiner.name().c_str());

  // examiner.correct_answer_ - not available, private!
  // Ha! I'll try to X-Ray him then.
  printf("X-Ray'ing the examiner...\n");
  // Note: this is an attempt to violate encapsulation
  for (size_t i = 0; i < sizeof(examiner); i++)
    printf("%c", ((const char*)&examiner)[i]);
  printf("\n");

  printf("And what should I answer?..\n");
  const char* answer = (const char*)&examiner;  // Don't do that!
  if (examiner.Answer(answer)) {
    printf("Yeeeeeah!\n");
  } else {
    printf("Oh no! :(\n");
  }
}

int main() {
  Examiner beck("Dmitry Vladimirovich");
  beck.set_correct_answer("Almost but not quite entirely unlike tea");
  StudentDuringTheExam(beck);
  return 0;
}
