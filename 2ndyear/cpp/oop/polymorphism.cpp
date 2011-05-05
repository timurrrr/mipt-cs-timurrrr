#include <stdio.h>

#include <vector>

class Animal {
 public:
  virtual void Feed() = 0;  // 'pure virtual' function
};

class Cat : public Animal {
 public:
  virtual void Feed() {
    printf("Meeeaoo!\n");  // "Thank you" in cat language.
  }
};

class Dog : public Animal {
 public:
  virtual void Feed() {
    printf("Bark! Bark!\n");  // "Thank you, Master" in dog language.
  }
};

class Cow : public Animal {
 public:
  virtual void Feed() {
    printf("Mmmmoooo!\n");  // "The fresh grass is better but that'll do"
  }
};

void FeedSomeAnimals(const std::vector<Animal*> animals) {
  // This function doesn't know anything about the types of animals[]
  // The interface is enough!
  for (size_t i = 0; i < animals.size(); i++)
    animals[i]->Feed();
}

int main() {
  std::vector<Animal*> animals;
  animals.push_back(new Cat);
  animals.push_back(new Cow);
  animals.push_back(new Dog);

  FeedSomeAnimals(animals);

  for (size_t i = 0; i < animals.size(); i++)
    delete animals[i];
  return 0;
}
