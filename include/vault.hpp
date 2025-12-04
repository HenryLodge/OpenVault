#ifndef VAULT_HPP
#define VAULT_HPP

#include "password_entry.hpp"
#include "cryptography.hpp"
#include "exceptions.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <cstdint>

class Vault {
  private:
    // all vault vars
    static const int MAGIC_SIZE = 4;
    static const int SALT_SIZE = 16;
    static const int HASH_SIZE = 32;
    static const int HEADER_SIZE = 128;

    std::string filename;
    std::string master_password_hash;
    std::vector<uint8_t> salt;
    std::vector<uint8_t> encryption_key;
    int iterations;
    bool isOpen;
    
    std::unique_ptr<CryptoManager> cryptography;
    std::map<int, PasswordEntry> entries;
    int nextId;

    // write header to file
    void writeHeader(std::ofstream &file);
    // read header
    void readHeader(std::ifstream &file);

    // hash password
    std::vector<uint8_t> hashPassword(const std::string &password);
    // check password against hash
    void verifyPassword(const std::string &password);

    // convert before encrypyt
    std::vector<uint8_t> serializeEntries();
    // decrypt back into object to return
    void deserializeEntries(const std::vector<uint8_t> &data);

    // copy magic number to buffer
    void copyMagicNumber(char *dest, const char *src);

  public:
    // construct
    explicit Vault(const std::string &filename);
    // destruct
    ~Vault();

    // delete when copy
    Vault(const Vault &) = delete;
    Vault &operator=(const Vault &) = delete;

    // full vault operations
    void create(const std::string &masterPassword);
    void open(const std::string &masterPassword);
    void save();
    void close();

    // entry operations
    int addEntry(const PasswordEntry &entry);
    PasswordEntry getEntry(int id) const;
    std::vector<PasswordEntry> getAllEntries() const;
    void updateEntry(const PasswordEntry &entry);
    void deleteEntry(int id);

    // search entries for specific entry to return
    std::vector<PasswordEntry> searchByService(const std::string &query) const;
    std::vector<PasswordEntry> searchByCategory(const std::string &category) const;

    // check if vault unlocked
    bool isVaultOpen() const { 
      return isOpen;
    }
    // get total num entries 
    int getEntryCount() const { 
      return entries.size();
    }
    // get name of file
    std::string getFilename() const { 
      return filename;
    }
};

#endif
