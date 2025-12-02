#ifndef CRYPTOGRAPHY_HPP
#define CRYPTOGRAPHY_HPP

#include <vector>
#include <string>
#include <cstdint>

class CryptoManager {
private:
  // vars
  static const int SALT_SIZE = 16;
  static const int IV_SIZE = 16;
  static const int KEY_SIZE = 32;
  static const int ITERATIONS = 100000;

public:
  // construct
  CryptoManager();
  // desctruct
  ~CryptoManager();

  // gen random salt value
  std::vector<uint8_t> generateSalt();
  
  // gen random IV value
  std::vector<uint8_t> generateIV();
  
  // derive encryption key from password
  // use PBKDF2
  std::vector<uint8_t> deriveKey(const std::string& password, const std::vector<uint8_t>& salt);
  
  // encrypt with AES-256-CBC
  std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
  
  // decrypt
  std::vector<uint8_t> decrypt(const std::vector<uint8_t>& encryptedtext, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
  
  // wipe
  void wipe(void* ptr, size_t size);
};

#endif
