
#include <iostream>

#include "pcsc-cpp/pcsc-cpp.hpp"

int main(int argc, char** argv) {
  std::cout << "Hello!" << std::endl;

  auto readers = pcsc_cpp::listReaders();
  std::cout << "readers = " << std::endl;
  for (auto reader : readers) {
    std::cout << " - " << reader.statusString() << std::endl;
  }


  return 0;
}

