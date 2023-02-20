
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

std::string bytes_to_str(std::vector<unsigned char> some_bytes) {
  std::string str = "";
  for (auto b : some_bytes) {
    str += one_byte_to_string(b);
  }
  return str;
}

std::vector<unsigned char> do_tx(pcsc_cpp::SmartCard::ptr& card_ptr, std::vector<unsigned char> apdu_cmd_bytes) {
  auto directory_c = pcsc_cpp::CommandApdu::fromBytes(apdu_cmd_bytes);
  auto tx_guard2 = card_ptr->beginTransaction();
  auto response = card_ptr->transmit(directory_c);
  return response.toBytes();
}


int main(int argc, char** argv) {
  auto readers = pcsc_cpp::listReaders();
  std::cout << "=== readers ===" << std::endl;
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

        // {
        //   std::vector<unsigned char> req {0x00, 0xa4, 0x00, 0x0c, /**/ 0x90, 0x00};
        //   auto resp = do_tx(card_ptr, req);
        //   std::cout << "req=" << bytes_to_str(req) << " resp=" << bytes_to_str(resp) << std::endl;
        // }

        std::vector<unsigned char> fcp_req {
          0x00, // Class byte, always 0
          0xA4, // Instruction: 0xA4 means Select (something), 0xB2 means read a record
          0x04, // Instruction parameter 1
          0x00, // Instruction parameter 2
          0x07, // number of data bytes (next N bytes)
          0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, // VISA AID
          // 0x00, // Max number of response bytes we can accept; 0 == 256
        };
        auto fcp_resp = do_tx(card_ptr, fcp_req);
        std::cout << "fcp_req=" << bytes_to_str(fcp_req) << " fcp_resp=" << bytes_to_str(fcp_resp) << std::endl;
        std::cout << std::endl;

        // // We want to try to read everything; when we get back 0x90 (OK) or 0x61 (more data available) as first byte of response we found something!
        // for (unsigned char p1=0x00; p1 < 0xff; p1 += 1) {
        //   for (unsigned char p2=0x00; p2 < 0x0f; p2 += 1) {
        //     std::vector<unsigned char> req {
        //       0x00, // Class byte, always 0
        //       0xB2, // Instruction: 0xA4 means Select (something), 0xB2 means read a record
        //       p1, // Instruction parameter 1
        //       p2, // Instruction parameter 2
        //       0x00, // number of data bytes (next N bytes)
        //       // No data bytes
        //       0x00, // Max number of response bytes we can accept; 0 == 256
        //     };
        //     try {
        //       auto resp = do_tx(card_ptr, req);
        //       if (resp[0] == 0x90 || resp[0] == 0x61) {
        //         std::cout << "p1=" << one_byte_to_string(p1) << " p2=" << one_byte_to_string(p2) << std::endl;
        //         std::cout << "req=" << bytes_to_str(req) << " resp=" << bytes_to_str(resp) << std::endl;
        //         std::cout << std::endl;
        //       }
        //     }
        //     catch (...) {
        //       // Guess who doesn't care?
        //     }
        //   }
        // }



      }
    }
  }


  return 0;
}

