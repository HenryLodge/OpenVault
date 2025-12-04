#ifndef PASSWORD_ENTRY_CXXTEST_HPP
#define PASSWORD_ENTRY_CXXTEST_HPP

#include <cxxtest/TestSuite.h>
#include "password_entry.hpp"
#include "exceptions.hpp"
#include <unistd.h>

class PasswordEntryTestSuite : public CxxTest::TestSuite {
public:
    
  void testDefaultConstructor() {
    PasswordEntry entry;
    
    TS_ASSERT_EQUALS(entry.getId(), 0);
    TS_ASSERT_EQUALS(entry.getService(), "");
    TS_ASSERT_EQUALS(entry.getUsername(), "");
    TS_ASSERT_EQUALS(entry.getPassword(), "");
  }
  
  void testParameterizedConstructor() {
    PasswordEntry entry(1, "service", "username", "password");
    
    TS_ASSERT_EQUALS(entry.getId(), 1);
    TS_ASSERT_EQUALS(entry.getService(), "service");
    TS_ASSERT_EQUALS(entry.getUsername(), "username");
    TS_ASSERT_EQUALS(entry.getPassword(), "password");
  }
  
  void testCopyConstructor() {
    PasswordEntry entry1(1, "service", "username", "password");
    PasswordEntry entry2(entry1);
    
    TS_ASSERT_EQUALS(entry2.getId(), entry1.getId());
    TS_ASSERT_EQUALS(entry2.getService(), entry1.getService());
    TS_ASSERT_EQUALS(entry2.getUsername(), entry1.getUsername());
    TS_ASSERT_EQUALS(entry2.getPassword(), entry1.getPassword());
  }
  
  void testAssignmentOperator() {
    PasswordEntry entry1(1, "service", "username", "password");
    PasswordEntry entry2;
    
    entry2 = entry1;
    
    TS_ASSERT_EQUALS(entry2.getId(), entry1.getId());
    TS_ASSERT_EQUALS(entry2.getService(), entry1.getService());
  }
  
  void testEqualityOperator() {
    PasswordEntry entry1(1, "service", "username", "password");
    PasswordEntry entry2(1, "test", "test", "test");
    PasswordEntry entry3(2, "service", "username", "password");
    
    TS_ASSERT(entry1 == entry2);
    TS_ASSERT(!(entry1 == entry3));
  }
  
  void testLessThanOperator() {
    PasswordEntry entry1(1, "a", "user1", "pass1");
    PasswordEntry entry2(2, "z", "user2", "pass2");
    
    TS_ASSERT(entry1 < entry2);
    TS_ASSERT(!(entry2 < entry1));
  }
  
  void testSetters() {
    PasswordEntry entry;
    
    entry.setService("service");
    entry.setUsername("username");
    entry.setPassword("password");
    entry.setUrl("url");
    entry.setCategory("category");
    entry.setNotes("notes");
    
    TS_ASSERT_EQUALS(entry.getService(), "service");
    TS_ASSERT_EQUALS(entry.getUsername(), "username");
    TS_ASSERT_EQUALS(entry.getPassword(), "password");
    TS_ASSERT_EQUALS(entry.getUrl(), "url");
    TS_ASSERT_EQUALS(entry.getCategory(), "category");
    TS_ASSERT_EQUALS(entry.getNotes(), "notes");
  }
  
  void testSerialization() {
    PasswordEntry entry1(1, "service", "username", "password");
    entry1.setUrl("url");
    entry1.setCategory("category");
    entry1.setNotes("notes");
    
    std::string serialized = entry1.serialize();
    
    TS_ASSERT(serialized.find("service") != std::string::npos);
    TS_ASSERT(serialized.find("username") != std::string::npos);
    TS_ASSERT(serialized.find("password") != std::string::npos);
    
    PasswordEntry entry2 = PasswordEntry::deserialize(serialized);
    
    TS_ASSERT_EQUALS(entry2.getId(), entry1.getId());
    TS_ASSERT_EQUALS(entry2.getService(), entry1.getService());
    TS_ASSERT_EQUALS(entry2.getUsername(), entry1.getUsername());
    TS_ASSERT_EQUALS(entry2.getPassword(), entry1.getPassword());
    TS_ASSERT_EQUALS(entry2.getUrl(), entry1.getUrl());
    TS_ASSERT_EQUALS(entry2.getCategory(), entry1.getCategory());
    TS_ASSERT_EQUALS(entry2.getNotes(), entry1.getNotes());
  }
  
  void testUpdateModified() {
    PasswordEntry entry;
    time_t before = entry.getModified();
    
    sleep(1);
    entry.setPassword("newpass");
    
    time_t after = entry.getModified();
    
    TS_ASSERT(after > before);
  }
  
  void testInvalidDeserialization() {
    std::string invalid = "test|test|test";
    
    bool thrown = false;
    try {
      PasswordEntry::deserialize(invalid);
    } catch (const EntryException&) {
      thrown = true;
    }
    TS_ASSERT(thrown);
  }
};

#endif
