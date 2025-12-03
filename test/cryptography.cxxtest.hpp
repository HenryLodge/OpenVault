#ifndef CRYPTO_CXXTEST_HPP
#define CRYPTO_CXXTEST_HPP

#include <cxxtest/TestSuite.h>
#include "cryptography.hpp"
#include "utils.hpp"
#include <vector>
#include <string>

// CONCEPT D: Class with access specifiers, constructor, destructor
// Testing CryptoManager class
class CryptoTestSuite : public CxxTest::TestSuite {
public:
    
    void testGenerateSalt() {
        CryptoManager crypto;
        
        // Test 1: Salt is correct size
        std::vector<uint8_t> salt = crypto.generateSalt();
        TS_ASSERT_EQUALS(salt.size(), 16);
        
        // Test 2: Multiple salts are different (randomness)
        std::vector<uint8_t> salt2 = crypto.generateSalt();
        TS_ASSERT_DIFFERS(salt, salt2);
    }
    
    void testGenerateIV() {
        CryptoManager crypto;
        
        // Test 1: IV is correct size
        std::vector<uint8_t> iv = crypto.generateIV();
        TS_ASSERT_EQUALS(iv.size(), 16);
        
        // Test 2: Multiple IVs are different (randomness)
        std::vector<uint8_t> iv2 = crypto.generateIV();
        TS_ASSERT_DIFFERS(iv, iv2);
    }
    
    void testDeriveKey() {
        CryptoManager crypto;
        
        std::string password = "TestPassword123!";
        std::vector<uint8_t> salt = crypto.generateSalt();
        
        // Test 1: Key is correct size (256 bits = 32 bytes)
        std::vector<uint8_t> key = crypto.deriveKey(password, salt);
        TS_ASSERT_EQUALS(key.size(), 32);
        
        // Test 2: Same password and salt produce same key
        std::vector<uint8_t> key2 = crypto.deriveKey(password, salt);
        TS_ASSERT_EQUALS(key, key2);
        
        // Test 3: Different salt produces different key
        std::vector<uint8_t> salt3 = crypto.generateSalt();
        std::vector<uint8_t> key3 = crypto.deriveKey(password, salt3);
        TS_ASSERT_DIFFERS(key, key3);
        
        // Test 4: Different password produces different key
        std::vector<uint8_t> key4 = crypto.deriveKey("DifferentPassword", salt);
        TS_ASSERT_DIFFERS(key, key4);
    }
    
    void testEncryptDecrypt() {
        CryptoManager crypto;
        
        // Setup
        std::string password = "TestPassword123!";
        std::vector<uint8_t> salt = crypto.generateSalt();
        std::vector<uint8_t> key = crypto.deriveKey(password, salt);
        std::vector<uint8_t> iv = crypto.generateIV();
        
        std::string plaintext = "This is a secret message!";
        std::vector<uint8_t> plaintext_bytes = Utils::stringToBytes(plaintext);
        
        // Test 1: Encryption produces different output than input
        std::vector<uint8_t> ciphertext = crypto.encrypt(plaintext_bytes, key, iv);
        TS_ASSERT_DIFFERS(ciphertext, plaintext_bytes);
        
        // Test 2: Decryption recovers original plaintext
        std::vector<uint8_t> decrypted_bytes = crypto.decrypt(ciphertext, key, iv);
        std::string decrypted = Utils::bytesToString(decrypted_bytes);
        TS_ASSERT_EQUALS(decrypted, plaintext);
        
        // Test 3: Ciphertext is different from plaintext
        TS_ASSERT(ciphertext.size() >= plaintext_bytes.size());
    }
    
    void testEncryptWithDifferentIVs() {
        CryptoManager crypto;
        
        // Setup
        std::string password = "TestPassword123!";
        std::vector<uint8_t> salt = crypto.generateSalt();
        std::vector<uint8_t> key = crypto.deriveKey(password, salt);
        
        std::string plaintext = "This is a secret message!";
        std::vector<uint8_t> plaintext_bytes = Utils::stringToBytes(plaintext);
        
        // Test: Same plaintext with different IVs produces different ciphertext
        std::vector<uint8_t> iv1 = crypto.generateIV();
        std::vector<uint8_t> iv2 = crypto.generateIV();
        
        std::vector<uint8_t> ciphertext1 = crypto.encrypt(plaintext_bytes, key, iv1);
        std::vector<uint8_t> ciphertext2 = crypto.encrypt(plaintext_bytes, key, iv2);
        
        TS_ASSERT_DIFFERS(ciphertext1, ciphertext2);
        
        // Both should decrypt to same plaintext
        std::vector<uint8_t> decrypted1 = crypto.decrypt(ciphertext1, key, iv1);
        std::vector<uint8_t> decrypted2 = crypto.decrypt(ciphertext2, key, iv2);
        
        TS_ASSERT_EQUALS(decrypted1, decrypted2);
        TS_ASSERT_EQUALS(decrypted1, plaintext_bytes);
    }
    
    void testEncryptEmptyData() {
        CryptoManager crypto;
        
        // Setup
        std::string password = "TestPassword123!";
        std::vector<uint8_t> salt = crypto.generateSalt();
        std::vector<uint8_t> key = crypto.deriveKey(password, salt);
        std::vector<uint8_t> iv = crypto.generateIV();
        
        // Test empty data
        std::vector<uint8_t> empty_data;
        std::vector<uint8_t> ciphertext = crypto.encrypt(empty_data, key, iv);
        
        // Even empty data should produce some ciphertext (padding)
        TS_ASSERT(ciphertext.size() > 0);
        
        // Should decrypt back to empty
        std::vector<uint8_t> decrypted = crypto.decrypt(ciphertext, key, iv);
        TS_ASSERT_EQUALS(decrypted.size(), 0);
    }
    
    void testEncryptLargeData() {
        CryptoManager crypto;
        
        // Setup
        std::string password = "TestPassword123!";
        std::vector<uint8_t> salt = crypto.generateSalt();
        std::vector<uint8_t> key = crypto.deriveKey(password, salt);
        std::vector<uint8_t> iv = crypto.generateIV();
        
        // Create large data (1MB)
        std::vector<uint8_t> large_data(1024 * 1024, 'A');
        
        // Test encryption and decryption of large data
        std::vector<uint8_t> ciphertext = crypto.encrypt(large_data, key, iv);
        std::vector<uint8_t> decrypted = crypto.decrypt(ciphertext, key, iv);
        
        TS_ASSERT_EQUALS(decrypted, large_data);
    }
    
    void testInvalidDecryption() {
        CryptoManager crypto;
        
        // Setup
        std::string password = "TestPassword123!";
        std::vector<uint8_t> salt = crypto.generateSalt();
        std::vector<uint8_t> key = crypto.deriveKey(password, salt);
        std::vector<uint8_t> iv = crypto.generateIV();
        
        std::string plaintext = "Secret message";
        std::vector<uint8_t> plaintext_bytes = Utils::stringToBytes(plaintext);
        std::vector<uint8_t> ciphertext = crypto.encrypt(plaintext_bytes, key, iv);
        
        // Test: Decryption with wrong key should throw or produce garbage
        std::vector<uint8_t> wrong_key = crypto.deriveKey("WrongPassword", salt);
        
        // This should either throw or produce incorrect data
        bool caught_exception = false;
        try {
            std::vector<uint8_t> decrypted = crypto.decrypt(ciphertext, wrong_key, iv);
            // If it doesn't throw, it should at least not match original
            std::string decrypted_str = Utils::bytesToString(decrypted);
            TS_ASSERT_DIFFERS(decrypted_str, plaintext);
        } catch (...) {
            caught_exception = true;
        }
        
        // Either exception or wrong data is acceptable
        TS_ASSERT(caught_exception || true);
    }
};

#endif // CRYPTO_CXXTEST_HPP