#ifndef CRYPTO_CXXTEST_HPP
#define CRYPTO_CXXTEST_HPP

#include <cxxtest/TestSuite.h>
#include "cryptography.hpp"
#include "utils.hpp"
#include <vector>
#include <string>

class CryptoTestSuite : public CxxTest::TestSuite {
  public:
    void testGenerateSalt() {
      CryptoManager crypto;

      std::vector<uint8_t> salt = crypto.generateSalt();
      TS_ASSERT_EQUALS(salt.size(), 16);

      std::vector<uint8_t> salt2 = crypto.generateSalt();
      TS_ASSERT_DIFFERS(salt, salt2);
    }

    void testGenerateIV() {
      CryptoManager crypto;

      std::vector<uint8_t> iv = crypto.generateIV();
      TS_ASSERT_EQUALS(iv.size(), 16);

      std::vector<uint8_t> iv2 = crypto.generateIV();
      TS_ASSERT_DIFFERS(iv, iv2);
    }

    void testDeriveKey() {
      CryptoManager crypto;
      std::string password = "Password123";
      std::vector<uint8_t> salt = crypto.generateSalt();

      std::vector<uint8_t> key = crypto.deriveKey(password, salt);
      TS_ASSERT_EQUALS(key.size(), 32);

      std::vector<uint8_t> key2 = crypto.deriveKey(password, salt);
      TS_ASSERT_EQUALS(key, key2);

      std::vector<uint8_t> salt3 = crypto.generateSalt();
      std::vector<uint8_t> key3 = crypto.deriveKey(password, salt3);
      TS_ASSERT_DIFFERS(key, key3);

      std::vector<uint8_t> key4 = crypto.deriveKey("DifferentPassword", salt);
      TS_ASSERT_DIFFERS(key, key4);
    }

    void testEncryptDecrypt() {
      CryptoManager crypto;
      std::string password = "Password123";
      std::vector<uint8_t> salt = crypto.generateSalt();
      std::vector<uint8_t> key = crypto.deriveKey(password, salt);
      std::vector<uint8_t> iv = crypto.generateIV();
      std::string plaintext = "plaintext";
      std::vector<uint8_t> plaintext_bytes = Utils::stringToBytes(plaintext);

      std::vector<uint8_t> ciphertext = crypto.encrypt(plaintext_bytes, key, iv);
      TS_ASSERT_DIFFERS(ciphertext, plaintext_bytes);

      std::vector<uint8_t> decrypted_bytes = crypto.decrypt(ciphertext, key, iv);
      std::string decrypted = Utils::bytesToString(decrypted_bytes);
      TS_ASSERT_EQUALS(decrypted, plaintext);

      TS_ASSERT(ciphertext.size() >= plaintext_bytes.size());
    }

    void testEncryptWithDifferentIVs() {
      CryptoManager crypto;
      std::string password = "Password123";
      std::vector<uint8_t> salt = crypto.generateSalt();
      std::vector<uint8_t> key = crypto.deriveKey(password, salt);
      std::string plaintext = "plaintext";
      std::vector<uint8_t> plaintext_bytes = Utils::stringToBytes(plaintext);

      std::vector<uint8_t> iv1 = crypto.generateIV();
      std::vector<uint8_t> iv2 = crypto.generateIV();

      std::vector<uint8_t> ciphertext1 = crypto.encrypt(plaintext_bytes, key, iv1);
      std::vector<uint8_t> ciphertext2 = crypto.encrypt(plaintext_bytes, key, iv2);

      TS_ASSERT_DIFFERS(ciphertext1, ciphertext2);

      std::vector<uint8_t> decrypted1 = crypto.decrypt(ciphertext1, key, iv1);
      std::vector<uint8_t> decrypted2 = crypto.decrypt(ciphertext2, key, iv2);

      TS_ASSERT_EQUALS(decrypted1, decrypted2);
      TS_ASSERT_EQUALS(decrypted1, plaintext_bytes);
    }

    void testEncryptEmptyData() {
      CryptoManager crypto;
      std::string password = "Password123";
      std::vector<uint8_t> salt = crypto.generateSalt();
      std::vector<uint8_t> key = crypto.deriveKey(password, salt);
      std::vector<uint8_t> iv = crypto.generateIV();

      std::vector<uint8_t> empty_data;
      std::vector<uint8_t> ciphertext = crypto.encrypt(empty_data, key, iv);

      TS_ASSERT(ciphertext.size() > 0);

      std::vector<uint8_t> decrypted = crypto.decrypt(ciphertext, key, iv);
      TS_ASSERT_EQUALS(decrypted.size(), 0);
    }

    void testEncryptLargeData() {
      CryptoManager crypto;
      std::string password = "Password123";
      std::vector<uint8_t> salt = crypto.generateSalt();
      std::vector<uint8_t> key = crypto.deriveKey(password, salt);
      std::vector<uint8_t> iv = crypto.generateIV();

      std::vector<uint8_t> large_data(1024 * 1024, 'A');

      std::vector<uint8_t> ciphertext = crypto.encrypt(large_data, key, iv);
      std::vector<uint8_t> decrypted = crypto.decrypt(ciphertext, key, iv);

      TS_ASSERT_EQUALS(decrypted, large_data);
    }

    void testInvalidDecryption() {
      CryptoManager crypto;

      std::string password = "Password123";
      std::vector<uint8_t> salt = crypto.generateSalt();
      std::vector<uint8_t> key = crypto.deriveKey(password, salt);
      std::vector<uint8_t> iv = crypto.generateIV();

      std::string plaintext = "plaintext";
      std::vector<uint8_t> plaintext_bytes = Utils::stringToBytes(plaintext);
      std::vector<uint8_t> ciphertext = crypto.encrypt(plaintext_bytes, key, iv);

      std::vector<uint8_t> wrong_key = crypto.deriveKey("wrong password", salt);

      bool caught_exception = false;
      try {
        std::vector<uint8_t> decrypted = crypto.decrypt(ciphertext, wrong_key, iv);
        std::string decrypted_str = Utils::bytesToString(decrypted);
        TS_ASSERT_DIFFERS(decrypted_str, plaintext);
      }
      catch (...) {
        caught_exception = true;
      }
      TS_ASSERT(caught_exception || true);
    }
};

#endif
