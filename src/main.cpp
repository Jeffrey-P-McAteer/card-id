
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

std::string resp_bytestr_to_msg(std::string err_msg_bytes) {
  std::transform(err_msg_bytes.begin(), err_msg_bytes.end(), err_msg_bytes.begin(), [](char const &c) {
      return std::toupper(c);
  });
  if (err_msg_bytes == "6A83") {
    return "Record not found";
  }
  else if (err_msg_bytes == "6A83") {
    return "Record not found";
  }
  return err_msg_bytes;
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
  std::cout << "RX=" << bytes_to_str(response) << std::endl;
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
          0x04, // Instruction parameter 1
          0x00, // Instruction parameter 2
          0x07, // number of data bytes (next N bytes)
          0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, // VISA AID (application identifier)
          // 0x00, // Max number of response bytes we can accept; 0 == 256; pcsc_cpp automatically calculates this for us
        };


        do_tx_noisy(card_ptr, req02);

        // // Now we _ought_ to be able to read records:
        // for (unsigned char sfi = 0x01; sfi <= 0x1f; sfi += 0x01) {
        //   for (unsigned char rec = 0x01; rec <= 0x10; rec += 0x01) {
        //     std::cout << "sfi=" << one_byte_to_string(sfi) << " rec=" << one_byte_to_string(rec) << std::endl;

        //     std::vector<unsigned char> read_req {
        //       0x00, // Class byte, always 0
        //       0xB2, // Instruction: 0xA4 means Select (something), 0xB2 means read a record
        //       rec, // Instruction parameter 1 (record number)
        //       (unsigned char) ((sfi << 3) | 4), // Instruction parameter 2 (not even gonna pretend to understand why we're shifting this around)
        //       0x00, // number of data bytes (next N bytes)
        //     };
        //     std::cout << "read_req=" << bytes_to_str(read_req) << std::endl;
        //     try {
        //       auto read_resp = do_tx(card_ptr, read_req, true);
        //       std::cout << "read_resp=" << bytes_to_str(read_resp) << " (" << resp_bytestr_to_msg(bytes_to_str(read_resp)) << ")" << std::endl;
        //       std::cout << std::endl;
        //       if (read_resp[0] == 0x6A && read_resp[1] == 0x83) {
        //         rec = 0x12; // end inner loop
        //       }

        //     }
        //     catch (...) {
        //       std::cout << "Exception!" << std::endl;
        //     }

        //     std::cout << std::endl;

        //   }
        // }




      }
    }
  }


  return 0;
}

