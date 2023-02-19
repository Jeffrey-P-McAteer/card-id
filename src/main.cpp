
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

        // auto command = pcsc_cpp::CommandApdu::fromBytes({0x2, 0x1, 0x3, 0x4}); // ????
        // auto transactionGuard = card_ptr->beginTransaction();
        // auto response = card_ptr->transmit(command); // Returns ResponseApdu

        // //std::cout << "response=" << response.toBytes() << std::endl;
        // auto resp_bytes = response.toBytes();
        // std::string result{};
        // result.reserve(4096);
        // for (auto byte_b : resp_bytes) {
        //   result += one_byte_to_string(byte_b);
        // }

        // std::cout << "response=" << result << std::endl;

        // Get master file
        auto masters_c = pcsc_cpp::CommandApdu::fromBytes({0x00, 0xa4, 0x00, 0x0c, /**/ 0x90, 0x00});
        auto tx_guard = card_ptr->beginTransaction();
        auto response = card_ptr->transmit(masters_c);
        auto resp_bytes = response.toBytes();

        std::string result{};
        result.reserve(4096);
        for (auto byte_b : resp_bytes) {
          result += one_byte_to_string(byte_b);
        }
        std::cout << "Master File result=" << result << std::endl;

        // Select directory
        auto directory_c = pcsc_cpp::CommandApdu::fromBytes({0x00, 0xa4, 0x01, 0x0c, 0x02, 0xee, 0xee, /**/ 0x90, 0x00});
        auto tx_guard2 = card_ptr->beginTransaction();
        response = card_ptr->transmit(directory_c);
        resp_bytes = response.toBytes();

        std::string result2{};
        result2.reserve(4096);
        for (auto byte_b : resp_bytes) {
          result2 += one_byte_to_string(byte_b);
        }
        std::cout << "Directory result2=" << result2 << std::endl;

        // Select certificate file
        auto certificate_c = pcsc_cpp::CommandApdu::fromBytes({0x00, 0xa4, 0x02, 0x0c, 0x02, 0xaa, 0xce, /**/ 0x90, 0x00});
        auto tx_guard3 = card_ptr->beginTransaction();
        response = card_ptr->transmit(certificate_c);
        resp_bytes = response.toBytes();

        std::string result3{};
        result3.reserve(4096);
        for (auto byte_b : resp_bytes) {
          result3 += one_byte_to_string(byte_b);
        }
        std::cout << "Certificate result3=" << result3 << std::endl;

        




      }
    }
  }


  return 0;
}

