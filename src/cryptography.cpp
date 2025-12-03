#include "cryptography.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <cstring>
#include <stdexcept>
#include <iostream>

// constructor
// init openssl
CryptoManager::CryptoManager() {
  OpenSSL_add_all_algorithms();
  ERR_load_crypto_strings();
}

// destructor
CryptoManager::~CryptoManager() {
  EVP_cleanup();
  ERR_free_strings();
}

// gen random salt value
std::vector<uint8_t> CryptoManager::generateSalt() {
  std::vector<uint8_t> salt(SALT_SIZE);
  
  // fill with random bytes
  if (RAND_bytes(salt.data(), SALT_SIZE) != 1) {
    throw std::runtime_error("generateSalt() failed, error");
  }
  
  return salt;
}

// gen random IV value
std::vector<uint8_t> CryptoManager::generateIV() {
  std::vector<uint8_t> iv(IV_SIZE);

  // fill with random bytes
  if (RAND_bytes(iv.data(), IV_SIZE) != 1) {
    throw std::runtime_error("generateIV() failed, error");
  }
  
  return iv;
}

// derive encryption key from password
// use PBKDF2
std::vector<uint8_t> CryptoManager::deriveKey(const std::string& password, const std::vector<uint8_t>& salt) {
  std::vector<uint8_t> key(KEY_SIZE);
  
  // get priv key from password, PBKDF2
  if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt.data(), salt.size(), ITERATIONS, EVP_sha256(), KEY_SIZE, key.data()) != 1) {
    throw std::runtime_error("deriveKey() failed, error");
  }
  
  return key;
}

// encrypt with AES-256-CBC
std::vector<uint8_t> CryptoManager::encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
  // new context, init encrypt using priv key and iv
  EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();
  if (EVP_EncryptInit_ex(context, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
    EVP_CIPHER_CTX_free(context);
    throw std::runtime_error("error initializing");
  }
  
  // buffer for encrypted text 
  std::vector<uint8_t> encryptedtext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
  int length = 0;
  int ciphertext_length = 0;
  
  // encrypt plaintext
  if (EVP_EncryptUpdate(context, encryptedtext.data(), &length, plaintext.data(), plaintext.size()) != 1) {
    EVP_CIPHER_CTX_free(context);
    throw std::runtime_error("error ecnrypting");
  }

  ciphertext_length = length;
  if (EVP_EncryptFinal_ex(context, encryptedtext.data() + length, &length) != 1) {
    EVP_CIPHER_CTX_free(context);
    throw std::runtime_error("error adding padding to end of buffer");
  }
  ciphertext_length += length;
  
  EVP_CIPHER_CTX_free(context);
  // remove any unencrypted data in buffer
  encryptedtext.resize(ciphertext_length);
  return encryptedtext;
}

// decrypt
// literally same as encrypt() but decrypting
std::vector<uint8_t> CryptoManager::decrypt(const std::vector<uint8_t>& encryptedtext, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
  // new context, init encrypt using priv key and iv
  EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();
  if (EVP_DecryptInit_ex(context, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
    EVP_CIPHER_CTX_free(context);
    throw std::runtime_error("error initializing");
  }
  
  // buffer for plaintext
  std::vector<uint8_t> plaintext(encryptedtext.size());
  int length = 0;
  int plaintext_length = 0;
  
  // decrypt encrypted text
  if (EVP_DecryptUpdate(context, plaintext.data(), &length, encryptedtext.data(), encryptedtext.size()) != 1) {
    EVP_CIPHER_CTX_free(context);
    throw std::runtime_error("error decrypting");
  }

  plaintext_length = length;
  if (EVP_DecryptFinal_ex(context, plaintext.data() + length, &length) != 1) {
    EVP_CIPHER_CTX_free(context);
    throw std::runtime_error("error adding padding to end of buffer");
  }
  plaintext_length += length;
  
  EVP_CIPHER_CTX_free(context);
  // remove any unencrypted data in buffer
  plaintext.resize(plaintext_length);
  return plaintext;
}

// wipe
void CryptoManager::wipe(void* ptr, size_t size) {
  if (ptr) {
    OPENSSL_cleanse(ptr, size);
  }
}
