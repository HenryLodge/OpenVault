#include "vault.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <cstring>
#include <openssl/sha.h>

// constructor
Vault::Vault(const std::string &filename) : filename(filename), iterations(100000), isOpen(false), cryptography(std::make_unique<CryptoManager>()), nextId(1) {
}

// destructor
Vault::~Vault() {
  if (isOpen) {
    try {
      save();
      close();
    }
    catch (...) {
      // none
    }
  }

  // wipe key
  if (!encryption_key.empty()) {
    (*cryptography).wipe(encryption_key.data(), encryption_key.size());
  }
}

// copy magic number
void Vault::copyMagicNumber(char *dest, const char *src) {
  std::memcpy(dest, src, MAGIC_SIZE);
}

// hash password
std::vector<uint8_t> Vault::hashPassword(const std::string &password) {
  std::vector<uint8_t> hash(HASH_SIZE);
  SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.length(), hash.data());
  return hash;
}

// check password against hash
void Vault::verifyPassword(const std::string &password) {
  std::vector<uint8_t> hash = hashPassword(password);
  std::vector<uint8_t> storedHash = Utils::stringToBytes(master_password_hash);

  if (hash != storedHash) {
    throw InvalidPasswordException("Incorrect password");
  }
}

//
void Vault::create(const std::string &masterPassword) {
  if (isOpen) {
    throw CustomException("Vault is already open");
  }

  // check if already exist
  std::ifstream check(filename);
  if (check.good()) {
    check.close();
    throw FileException("Vault file already exists: " + filename);
  }

  // generate salt and key
  salt = (*cryptography).generateSalt();
  encryption_key = (*cryptography).deriveKey(masterPassword, salt);
  master_password_hash = Utils::bytesToString(hashPassword(masterPassword));

  // Create empty vault file
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    throw FileException("Cannot create vault file: " + filename);
  }

  // write header
  writeHeader(file);

  int count = 0;
  std::vector<uint8_t> countBytes(sizeof(int));
  std::memcpy(countBytes.data(), &count, sizeof(int));

  auto iv = (*cryptography).generateIV();
  auto encrypted = (*cryptography).encrypt(countBytes, encryption_key, iv);

  file.write(reinterpret_cast<const char*>(iv.data()), iv.size());
  int encryptedSize = encrypted.size();
  file.write(reinterpret_cast<const char*>(&encryptedSize), sizeof(int));
  
  // Write encrypted data
  file.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
  file.close();
  isOpen = true;
}

// write header to file
void Vault::writeHeader(std::ofstream &file) {
  char magic[MAGIC_SIZE + 1] = {0};
  copyMagicNumber(magic, "OVLT");

  // write magic,version,salt,iters,hash
  int version = 1;
  file.write(magic, MAGIC_SIZE);
  file.write(reinterpret_cast<const char*>(&version), sizeof(int));
  file.write(reinterpret_cast<const char*>(salt.data()), salt.size());
  file.write(reinterpret_cast<const char*>(&iterations), sizeof(int));
  std::vector<uint8_t> hash = Utils::stringToBytes(master_password_hash);
  file.write(reinterpret_cast<const char*>(hash.data()), HASH_SIZE);

  // extra space
  int written = MAGIC_SIZE + sizeof(int) + SALT_SIZE + sizeof(int) + HASH_SIZE;
  int padding = HEADER_SIZE - written;
  std::vector<char> reserved(padding, 0);
  file.write(reserved.data(), padding);
}

// open vault
void Vault::open(const std::string &masterPassword) {
  if (isOpen) {
    throw CustomException("Vault is already open");
  }

  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw FileException("Cannot open vault file: " + filename);
  }

  try {
    readHeader(file);
    verifyPassword(masterPassword);

    // derive key
    encryption_key = (*cryptography).deriveKey(masterPassword, salt);

    // read iv,size,count
    int encryptedCountSize = 0;
    std::vector<uint8_t> iv(16);
    file.read(reinterpret_cast<char*>(iv.data()), iv.size());
    file.read(reinterpret_cast<char*>(&encryptedCountSize), sizeof(int));
    std::vector<uint8_t> encryptedCount(encryptedCountSize);
    file.read(reinterpret_cast<char*>(encryptedCount.data()), encryptedCountSize);
    auto decryptedCount = (*cryptography).decrypt(encryptedCount, encryption_key, iv);

    int count = 0;
    std::memcpy(&count, decryptedCount.data(), sizeof(int));

    // read entries
    entries.clear();
    nextId = 1;

    // for every entry
    for (int i = 0; i < count; ++i) {
      // read iv, size, encrypted
      int entrySize = 0;
      file.read(reinterpret_cast<char*>(iv.data()), iv.size());
      file.read(reinterpret_cast<char*>(&entrySize), sizeof(int));
      std::vector<uint8_t> encryptedEntry(entrySize);
      file.read(reinterpret_cast<char*>(encryptedEntry.data()), entrySize);

      // decrypt entry
      auto decryptedEntry = (*cryptography).decrypt(encryptedEntry, encryption_key, iv);
      std::string entryStr = Utils::bytesToString(decryptedEntry);

      // convert back to obj
      PasswordEntry entry = PasswordEntry::deserialize(entryStr);
      entries[entry.getId()] = entry;

      if (entry.getId() >= nextId) {
        nextId = entry.getId() + 1;
      }
    }

    // close
    file.close();
    isOpen = true;
  }
  catch (const CryptographyException &e) {
    file.close();
    throw CorruptedVaultException("Failed to decrypt vault data");
  }
  catch (const std::exception &e) {
    file.close();
    throw;
  }
}

// read header
void Vault::readHeader(std::ifstream &file) {
  // read magic num
  char magic[MAGIC_SIZE + 1] = {0};
  file.read(magic, MAGIC_SIZE);
  magic[MAGIC_SIZE] = '\0';

  if (std::strncmp(magic, "OVLT", MAGIC_SIZE) != 0) {
    throw CorruptedVaultException("Invalid vault file format");
  }

  // read version
  int version = 0;
  file.read(reinterpret_cast<char*>(&version), sizeof(int));
  if (version != 1) {
    throw CustomException("Unsupported vault version: " + std::to_string(version));
  }

  // read salt, iters, hash
  salt.resize(SALT_SIZE);
  file.read(reinterpret_cast<char*>(salt.data()), SALT_SIZE);
  file.read(reinterpret_cast<char*>(&iterations), sizeof(int));
  std::vector<uint8_t> hash(HASH_SIZE);
  file.read(reinterpret_cast<char*>(hash.data()), HASH_SIZE);
  master_password_hash = Utils::bytesToString(hash);

  // skip padding
  int read = MAGIC_SIZE + sizeof(int) + SALT_SIZE + sizeof(int) + HASH_SIZE;
  int skip = HEADER_SIZE - read;
  file.seekg(skip, std::ios::cur);
}

// save
void Vault::save()
{
  if (!isOpen) {
    throw CustomException("Vault is not open");
  }

  // temp
  std::string tempFile = filename + ".tmp";
  std::ofstream file(tempFile, std::ios::binary);

  if (!file) {
    throw FileException("Cannot write to vault file");
  }

  try {
    writeHeader(file);

    // write count
    int count = entries.size();
    std::vector<uint8_t> countBytes(sizeof(int));
    std::memcpy(countBytes.data(), &count, sizeof(int));

    auto iv = (*cryptography).generateIV();
    auto encrypted = (*cryptography).encrypt(countBytes, encryption_key, iv);

    // write iv, size count, count
    file.write(reinterpret_cast<const char*>(iv.data()), iv.size());
    int encryptedSize = encrypted.size();
    file.write(reinterpret_cast<const char*>(&encryptedSize), sizeof(int));
    file.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());

    // write every entry
    for (const auto &pair : entries) {
      const PasswordEntry &entry = pair.second;
      std::string serialized = entry.serialize();
      std::vector<uint8_t> entryBytes = Utils::stringToBytes(serialized);

      // encrypt
      iv = (*cryptography).generateIV();
      encrypted = (*cryptography).encrypt(entryBytes, encryption_key, iv);

      // write iv,size,entry
      int size = encrypted.size();
      file.write(reinterpret_cast<const char*>(iv.data()), iv.size());
      file.write(reinterpret_cast<const char*>(&size), sizeof(int));
      file.write(reinterpret_cast<const char*>(encrypted.data()), size);
    }

    file.close();

    // swap temp with old, remove old
    std::remove(filename.c_str());
    std::rename(tempFile.c_str(), filename.c_str());
  }
  catch (...) {
    file.close();
    std::remove(tempFile.c_str());
    throw;
  }
}

// close
void Vault::close() {
  if (isOpen) {
    // wipe key
    if (!encryption_key.empty()) {
      (*cryptography).wipe(encryption_key.data(), encryption_key.size());
      encryption_key.clear();
    }
    entries.clear();
    isOpen = false;
  }
}

// add entry 
int Vault::addEntry(const PasswordEntry &entry) {
  if (!isOpen) {
    throw CustomException("Vault is not open");
  }

  PasswordEntry newEntry = entry;
  newEntry.setId(nextId);

  entries[nextId] = newEntry;
  ++nextId;

  // save, return id num
  save();
  return newEntry.getId();
}

// get an entry
PasswordEntry Vault::getEntry(int id) const {
  if (!isOpen) {
    throw CustomException("Vault is not open");
  }

  auto entry_found = entries.find(id);
  if (entry_found == entries.end()) {
    throw EntryException("Entry not found: " + std::to_string(id));
  }

  return (*entry_found).second;
}

// get every entry
std::vector<PasswordEntry> Vault::getAllEntries() const {
  if (!isOpen) {
    throw CustomException("Vault is not open");
  }

  std::vector<PasswordEntry> entries_v;
  for (const auto &entry : entries) {
    entries_v.push_back(entry.second);
  }

  return entries_v;
}

// update an entry
void Vault::updateEntry(const PasswordEntry &entry) {
  if (!isOpen) {
    throw CustomException("Vault is not open");
  }

  auto entry_found = entries.find(entry.getId());
  if (entry_found == entries.end()) {
    throw EntryException("Entry not found: " + std::to_string(entry.getId()));
  }

  entries[entry.getId()] = entry;
  save();
}

// delete an entry
void Vault::deleteEntry(int id) {
  if (!isOpen) {
    throw CustomException("Vault is not open");
  }

  auto entry_found = entries.find(id);
  if (entry_found == entries.end()) {
    throw EntryException("Entry not found: " + std::to_string(id));
  }

  entries.erase(entry_found);
  save();
}

// search for entries by service
std::vector<PasswordEntry> Vault::searchByService(const std::string &query) const {
  if (!isOpen) {
    throw CustomException("Vault is not open");
  }

  std::vector<PasswordEntry> entry_found;

  for (const auto &entry : entries) {
    if (entry.second.getService().find(query) != std::string::npos)
    {
      entry_found.push_back(entry.second);
    }
  }

  return entry_found;
}

// search for entries by category
std::vector<PasswordEntry> Vault::searchByCategory(const std::string &category) const {
  if (!isOpen) {
    throw CustomException("Vault is not open");
  }

  std::vector<PasswordEntry> entry_found;

  for (const auto &entry : entries) {
    if (entry.second.getCategory() == category)
    {
      entry_found.push_back(entry.second);
    }
  }

  return entry_found;
}
