#include "cryptography.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <cstring>
#include <stdexcept>
#include <iostream>

// constructor
CryptoManager::CryptoManager() {
  //
}

// destructor
CryptoManager::~CryptoManager() {
  //
}

// gen random salt value
std::vector<uint8_t> CryptoManager::generateSalt() {
    return;
}

// gen random IV value
std::vector<uint8_t> CryptoManager::generateIV() {
    return;
}

// derive encryption key from password
// use PBKDF2
std::vector<uint8_t> CryptoManager::deriveKey(const std::string& password, const std::vector<uint8_t>& salt) {
    return;
}

// encrypt with AES-256-CBC
std::vector<uint8_t> CryptoManager::encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    return;
}

// decrypt
std::vector<uint8_t> CryptoManager::decrypt(const std::vector<uint8_t>& encryptedtext, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    return;
}

// wipe
void CryptoManager::wipe(void* ptr, size_t size) {
  //
}
