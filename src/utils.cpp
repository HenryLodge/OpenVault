#include "utils.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
// for turning off terminal echo
#include <termios.h>
#include <unistd.h>

namespace Utils {

  // convert string to bytes
  std::vector<uint8_t> stringToBytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
  }

  // convert bytes string
  std::string bytesToString(const std::vector<uint8_t>& bytes) {
    return std::string(bytes.begin(), bytes.end());
  }

  // convert bytes to hex string (for display)
  std::string bytesToHex(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t byte : bytes) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
  }

  // read password from stdin
  // turn off terminal echo first
  std::string readPassword(const std::string& prompt) {
    // from stackoverflow
    std::cout << prompt;
    
    termios oldSettings, newSettings;
    tcgetattr(STDIN_FILENO, &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
    
    std::string password;
    std::getline(std::cin, password);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    std::cout << std::endl;
    
    return password;
  }

}
