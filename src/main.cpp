
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <string>
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

std::string one_byte_to_string_chars(const unsigned char byte) {
    std::ostringstream stm ;
    stm << byte ;
    return stm.str();
}

std::string bytes_to_str_chars(std::vector<unsigned char> some_bytes) {
  std::string str = "";
  for (auto b : some_bytes) {
    str += one_byte_to_string_chars(b);
  }
  return str;
}

std::string resp_bytestr_to_msg(std::string err_msg_bytes, std::vector<unsigned char> some_bytes) {
  std::transform(err_msg_bytes.begin(), err_msg_bytes.end(), err_msg_bytes.begin(), [](char const &c) {
      return std::toupper(c);
  });
  // See https://www.eftlab.com/knowledge-base/complete-list-of-apdu-responses
  if (err_msg_bytes == "6A83") {
    return "Record not found";
  }
  else if (err_msg_bytes == "6A82") {
    return "File not found";
  }
  else if (err_msg_bytes == "6A81") {
    return "Function not supported";
  }
  return bytes_to_str_chars(some_bytes); // Could be useful data, by default decode to ASCII
}

std::vector<unsigned char> do_tx(pcsc_cpp::SmartCard::ptr& card_ptr, std::vector<unsigned char> apdu_cmd_bytes, bool useLe = false) {
  auto directory_c = pcsc_cpp::CommandApdu::fromBytes(apdu_cmd_bytes, useLe);
  auto tx_guard2 = card_ptr->beginTransaction();
  auto response = card_ptr->transmit(directory_c);
  return response.toBytes();
}

std::vector<unsigned char> do_tx_noisy(pcsc_cpp::SmartCard::ptr& card_ptr, std::vector<unsigned char> apdu_cmd_bytes, bool useLe = false) {
  std::cout << std::endl;
  std::cout << "TX=" << bytes_to_str(apdu_cmd_bytes) << " useLe=" << useLe << std::endl;
  auto response = do_tx(card_ptr, apdu_cmd_bytes, useLe);
  std::cout << "RX=" << bytes_to_str(response) << " (" << resp_bytestr_to_msg( bytes_to_str(response), response  ) << ")" << std::endl;
  //std::cout << "ASCII RX=" << bytes_to_str_chars(response) << std::endl;
  return response;
}


int main(int argc, char** argv) {
  auto readers = pcsc_cpp::listReaders();
  std::cout << "=== readers ===" << std::endl;
  for (auto reader : readers) {
    std::cout << " - " << reader.statusString() << std::endl;
    // Test if card exists, if so try to read from it
    if (reader.isCardInserted()) {
      std::cout << "Connecting to card..."<< std::endl;
      auto card_ptr = reader.connectToCard();
      if (card_ptr == nullptr) {
        std::cout << "Error connecting to card!"<< std::endl;
      }
      else {
        std::cout << "card_ptr=" << card_ptr.get() << std::endl;

        std::vector<unsigned char> fcp_req {
          0x00, // Class byte, always 0
          0xA4, // Instruction: 0xA4 means Select (something), 0xB2 means read a record
          0x04, // Instruction parameter 1
          0x00, // Instruction parameter 2
          0x07, // number of data bytes (next N bytes)
          0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, // VISA AID
          // 0x00, // Max number of response bytes we can accept; 0 == 256; pcsc_cpp automatically calculates this for us
        };
        // auto fcp_resp = do_tx(card_ptr, fcp_req);
        // std::cout << "fcp_req=" << bytes_to_str(fcp_req) << " fcp_resp=" << bytes_to_str(fcp_resp) << std::endl;
        // std::cout << std::endl;

        do_tx_noisy(card_ptr, fcp_req);

        std::vector<unsigned char> req02 {
          0x00, // Class byte, always 0
          0xA4, // Instruction: 0xA4 means Select (something), 0xB2 means read a record
          0x5F, // Instruction parameter 1
          0x20, // Instruction parameter 2
          0x00, // number of data bytes (next N bytes)
        };

        do_tx_noisy(card_ptr, req02, /*useLe:*/true);


        // Hey this gets something back!
        std::vector<unsigned char> req03 { // 00A404000E315041592E5359532E4444463031
          0x00, // Class byte, always 0
          0xA4, // Instruction: 0xA4 means Select (something), 0xB2 means read a record
          0x04, // Instruction parameter 1
          0x00, // Instruction parameter 2
          0x0E, // number of data bytes (next N bytes)
          0x31, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31
        };

        do_tx_noisy(card_ptr, req03, /*useLe:*/true);



      }
    }
  }


  return 0;
}

