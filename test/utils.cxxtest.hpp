#ifndef UTILS_CXXTEST_HPP
#define UTILS_CXXTEST_HPP

// #include <cxxtest/TestSuite.h>
#include <cxxtest/TestSuite.h>
#include "utils.hpp"
#include <vector>
#include <string>

class UtilsTestSuite : public CxxTest::TestSuite {
public:
    
    void testStringToBytes() {
        std::string input = "Hello, World!";
        std::vector<uint8_t> bytes = Utils::stringToBytes(input);
        
        // Test size matches
        TS_ASSERT_EQUALS(bytes.size(), input.size());
        
        // Test content matches
        for (size_t i = 0; i < input.size(); ++i) {
            TS_ASSERT_EQUALS(bytes[i], static_cast<uint8_t>(input[i]));
        }
    }
    
    void testBytesToString() {
        std::vector<uint8_t> bytes = {72, 101, 108, 108, 111}; // "Hello"
        std::string str = Utils::bytesToString(bytes);
        
        TS_ASSERT_EQUALS(str, "Hello");
    }
    
    void testStringBytesRoundTrip() {
        std::string original = "The quick brown fox jumps over the lazy dog";
        
        // Convert to bytes and back
        std::vector<uint8_t> bytes = Utils::stringToBytes(original);
        std::string recovered = Utils::bytesToString(bytes);
        
        TS_ASSERT_EQUALS(recovered, original);
    }
    
    void testBytesToHex() {
        std::vector<uint8_t> bytes = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
        std::string hex = Utils::bytesToHex(bytes);
        
        TS_ASSERT_EQUALS(hex, "0123456789abcdef");
    }
    
    void testBytesToHexAllValues() {
        // Test all byte values 0-255
        std::vector<uint8_t> bytes(256);
        for (int i = 0; i < 256; ++i) {
            bytes[i] = static_cast<uint8_t>(i);
        }
        
        std::string hex = Utils::bytesToHex(bytes);
        
        // Should be 512 characters (2 hex digits per byte)
        TS_ASSERT_EQUALS(hex.size(), 512);
        
        // Check a few specific values
        TS_ASSERT_EQUALS(hex.substr(0, 2), "00");     // First byte
        TS_ASSERT_EQUALS(hex.substr(30, 2), "0f");    // Byte 15
        TS_ASSERT_EQUALS(hex.substr(510, 2), "ff");   // Last byte
    }
    
    void testEmptyConversions() {
        // Test empty string to bytes
        std::string empty_str = "";
        std::vector<uint8_t> bytes = Utils::stringToBytes(empty_str);
        TS_ASSERT_EQUALS(bytes.size(), 0);
        
        // Test empty bytes to string
        std::vector<uint8_t> empty_bytes;
        std::string str = Utils::bytesToString(empty_bytes);
        TS_ASSERT_EQUALS(str.size(), 0);
        
        // Test empty bytes to hex
        std::string hex = Utils::bytesToHex(empty_bytes);
        TS_ASSERT_EQUALS(hex, "");
    }
    
    void testSpecialCharacters() {
        std::string special = "!@#$%^&*()_+-=[]{}|;':\",./<>?\\";
        
        std::vector<uint8_t> bytes = Utils::stringToBytes(special);
        std::string recovered = Utils::bytesToString(bytes);
        
        TS_ASSERT_EQUALS(recovered, special);
    }
    
    void testUnicodeCharacters() {
        // Test UTF-8 encoded strings
        std::string unicode = "Hello 世界 🌍";
        
        std::vector<uint8_t> bytes = Utils::stringToBytes(unicode);
        std::string recovered = Utils::bytesToString(bytes);
        
        TS_ASSERT_EQUALS(recovered, unicode);
    }
};

#endif // UTILS_CXXTEST_HPP