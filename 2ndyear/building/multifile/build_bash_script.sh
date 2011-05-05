#!/bin/bash

for FILE in *.cpp
do
  g++ -o ${FILE}.obj -c ${FILE}
done

g++ -o helloworld *.cpp.obj

# OR
# for FILE in *.cpp; do g++ -o ${FILE}.obj -c ${FILE}; done; g++ -o helloworld *.cpp.obj

# OR
# g++ -o helloworld *.cpp
