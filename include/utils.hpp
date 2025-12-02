#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <cstdint>

// helper functions
namespace Utils {
  // convert string to bytes
  std::vector<uint8_t> stringToBytes(const std::string& str);
  
  // convert bytes string
  std::string bytesToString(const std::vector<uint8_t>& bytes);
  
  // convert bytes to hex string (for display)
  std::string bytesToHex(const std::vector<uint8_t>& bytes);
  
  // read password from stdin (without echo)
  std::string readPassword(const std::string& input);
}

#endif
