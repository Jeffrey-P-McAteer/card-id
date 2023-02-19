
#include <iostream>
#include <cstdint>
#include <algorithm>
// #include <format> // WHY DOESN'T ANYONE HAVE C++20 <format> IMPLEMENTED 3 YEARS LATER???? Ugh


#include "pcsc-cpp/pcsc-cpp.hpp"
#include "pcsc-cpp/pcsc-cpp-utils.hpp"

std::string one_byte_to_string(const unsigned char byte) {
    std::ostringstream stm ;
    stm << std::hex << std::uppercase ;
    stm << std::setw(2) << std::setfill( '0' ) << unsigned( byte ) ;
    return stm.str();
}


int main(int argc, char** argv) {
  std::cout << "Hello!" << std::endl;

  auto readers = pcsc_cpp::listReaders();
  std::cout << "readers = " << std::endl;
  for (auto reader : readers) {
    std::cout << " - " << reader.statusString() << std::endl;
    // Test if card exists, if so try to read from it
    if (reader.isCardInserted()) {
      std::cout << "Connecting..."<< std::endl;
      auto card_ptr = reader.connectToCard();
      if (card_ptr == nullptr) {
        std::cout << "Error connecting to card!"<< std::endl;
      }
      else {
        std::cout << "card_ptr=" << card_ptr.get() << std::endl;

        auto command = pcsc_cpp::CommandApdu::fromBytes({0x2, 0x1, 0x3, 0x4}); // ????
        auto transactionGuard = card_ptr->beginTransaction();
        auto response = card_ptr->transmit(command); // Returns ResponseApdu

        //std::cout << "response=" << response.toBytes() << std::endl;
        auto resp_bytes = response.toBytes();
        std::string result{};
        result.reserve(4096);
        for (auto byte_b : resp_bytes) {
          result += one_byte_to_string(byte_b);
        }

        std::cout << "response=" << result << std::endl;


      }
    }
  }


  return 0;
}

