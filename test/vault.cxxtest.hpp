#ifndef VAULT_CXXTEST_HPP
#define VAULT_CXXTEST_HPP

#include <cxxtest/TestSuite.h>
#include "vault.hpp"
#include "password_entry.hpp"
#include "exceptions.hpp"
#include <cstdio>

class VaultTestSuite : public CxxTest::TestSuite {
private:
  std::string testVaultFile = "test_vault.ovault";
  std::string testPassword = "Password123";

public:
  void setUp() {
    std::remove(testVaultFile.c_str());
  }

  void tearDown() {
    std::remove(testVaultFile.c_str());
  }

  void testCreateVault() {
    Vault vault(testVaultFile);

    TS_ASSERT_THROWS_NOTHING(vault.create(testPassword));
    TS_ASSERT(vault.isVaultOpen());
  }

  void testCannotCreateDuplicateVault() {
    Vault vault1(testVaultFile);
    vault1.create(testPassword);
    vault1.close();

    Vault vault2(testVaultFile);
    bool caughtException = false;
    try {
      vault2.create(testPassword);
    } catch (const FileException& e) {
      caughtException = true;
    }
    TS_ASSERT(caughtException);
  }

  void testOpenVault() {
    Vault vault1(testVaultFile);
    vault1.create(testPassword);
    vault1.close();

    Vault vault2(testVaultFile);
    bool noException = true;
    try {
        vault2.open(testPassword);
    } catch (const std::exception& e) {
        noException = false;
    }
    
    TS_ASSERT(noException);
    TS_ASSERT(vault2.isVaultOpen());
  }

  void testOpenWithWrongPassword() {
    Vault vault1(testVaultFile);
    vault1.create(testPassword);
    vault1.close();

    Vault vault2(testVaultFile);
    bool caughtPasswordException = false;
    try {
      vault2.open("nottherightpassword");
    } catch (const InvalidPasswordException& e) {
      caughtPasswordException = true;
    }
    TS_ASSERT(caughtPasswordException);
    }

  void testAddEntry() {
    Vault vault(testVaultFile);
    vault.create(testPassword);

    PasswordEntry entry(0, "service", "username", "password");
    int id = vault.addEntry(entry);

    TS_ASSERT(id > 0);
    TS_ASSERT_EQUALS(vault.getEntryCount(), 1);
  }

  void testGetEntry() {
    Vault vault(testVaultFile);
    vault.create(testPassword);

    PasswordEntry entry(0, "service", "username", "password");
    int id = vault.addEntry(entry);

    PasswordEntry retrieved = vault.getEntry(id);
    TS_ASSERT_EQUALS(retrieved.getService(), "service");
    TS_ASSERT_EQUALS(retrieved.getUsername(), "username");
  }

  void testPersistence() {
    {
      Vault vault(testVaultFile);
      vault.create(testPassword);

      PasswordEntry entry(0, "service", "username", "password");
      vault.addEntry(entry);

      vault.close();
    }
    {
      Vault vault(testVaultFile);
      vault.open(testPassword);

      TS_ASSERT_EQUALS(vault.getEntryCount(), 1);

      auto entries = vault.getAllEntries();
      TS_ASSERT_EQUALS(entries.size(), 1);
      TS_ASSERT_EQUALS(entries[0].getService(), "service");
    }
  }

  void testDeleteEntry() {
    Vault vault(testVaultFile);
    vault.create(testPassword);

    PasswordEntry entry(0, "service", "username", "password");
    int id = vault.addEntry(entry);

    TS_ASSERT_EQUALS(vault.getEntryCount(), 1);

    vault.deleteEntry(id);

    TS_ASSERT_EQUALS(vault.getEntryCount(), 0);
    bool caughtEntryException = false;
    try {
      vault.getEntry(id);
    } catch (const EntryException& e) {
      caughtEntryException = true;
    }
    
    TS_ASSERT(caughtEntryException);
  }

  void testSearchByService() {
    Vault vault(testVaultFile);
    vault.create(testPassword);

    PasswordEntry entry1(0, "service1", "username1", "passwowrd1");
    PasswordEntry entry2(0, "service2", "username2", "passwowrd2");
    PasswordEntry entry3(0, "service3", "username3", "passwowrd3");

    vault.addEntry(entry1);
    vault.addEntry(entry2);
    vault.addEntry(entry3);

    auto results = vault.searchByService("service");
    TS_ASSERT_EQUALS(results.size(), 3);
  }

  void testMultipleEntries() {
    Vault vault(testVaultFile);
    vault.create(testPassword);

    for (int i = 0; i < 10; ++i) {
      PasswordEntry entry(0, "Service" + std::to_string(i), "user" + std::to_string(i), "pass");
      vault.addEntry(entry);
    }

    TS_ASSERT_EQUALS(vault.getEntryCount(), 10);

    auto allEntries = vault.getAllEntries();
    TS_ASSERT_EQUALS(allEntries.size(), 10);
  }
};

#endif
