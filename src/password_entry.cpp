#include "password_entry.hpp"
#include "exceptions.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>

// constructor 1, no vals passed
PasswordEntry::PasswordEntry() : id(0), service(""), username(""), password(""),
    url(""), notes(""), category(""), created(std::time(nullptr)), last_modified(std::time(nullptr)) {
}

// constructor 2, id,service,username,password vals passed
PasswordEntry::PasswordEntry(int id, const std::string& service, const std::string& username, const std::string& password)
    : id(id), service(service), username(username), password(password), url(""), notes(""),
      category(""), created(std::time(nullptr)), last_modified(std::time(nullptr)) {
}

// constructor 3, all vals passed
PasswordEntry::PasswordEntry(const PasswordEntry& other) : id(other.id), service(other.service), username(other.username),
    password(other.password), url(other.url), notes(other.notes),
    category(other.category), created(other.created), last_modified(other.last_modified) {
}

// destructor, overwrite password
PasswordEntry::~PasswordEntry() {
  if (!password.empty()) {
    volatile char* x = &password[0];
    for (size_t i = 0; i < password.size(); ++i) {
      x[i] = 0;
    }
  }
}

// = operator
PasswordEntry& PasswordEntry::operator=(const PasswordEntry& other) {
  if (this != &other) {
    id = other.id;
    service = other.service;
    username = other.username;
    password = other.password;
    url = other.url;
    notes = other.notes;
    category = other.category;
    created = other.created;
    last_modified = other.last_modified;
  }
  return *this;
}

// == operator
bool PasswordEntry::operator==(const PasswordEntry& other) const {
  return id == other.id;
}

// < operator
bool PasswordEntry::operator<(const PasswordEntry& other) const {
  return service < other.service;
}

// << operator
std::ostream& operator<<(std::ostream& os, const PasswordEntry& entry) {
  os << "ID: " << entry.id << "\n"
    << "Service: " << entry.service << "\n"
    << "Username: " << entry.username << "\n"
    << "Password: " << std::string(entry.password.length(), '*') << "\n";
  
  if (!entry.url.empty()) {
    os << "URL: " << entry.url << "\n";
  }
  if (!entry.category.empty()) {
    os << "Category: " << entry.category << "\n";
  }
  if (!entry.notes.empty()) {
    os << "Notes: " << entry.notes << "\n";
  }
  
  return os;
}

// update last modified time
void PasswordEntry::updateModified() {
  last_modified = std::time(nullptr);
}

// toString
std::string PasswordEntry::toString() const {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

// convert PasswordEntry to str
std::string PasswordEntry::serialize() const {
  std::ostringstream oss;
  oss << id << "|" << service << "|" << username << "|"
      << password << "|" << url << "|" << notes << "|"
      << category << "|" << created << "|" << last_modified;
  return oss.str();
}

// convert str to PasswordEntry
PasswordEntry PasswordEntry::deserialize(const std::string& data) {
  std::istringstream iss(data);
  std::string field;
  std::vector<std::string> fields;
  
  // delimiter is |
  while (std::getline(iss, field, '|')) {
    fields.push_back(field);
  }
  
  if (fields.size() != 9) {
    throw EntryException("Invalid serialized entry format");
  }
  
  PasswordEntry entry;
  entry.id = std::stoi(fields[0]);
  entry.service = fields[1];
  entry.username = fields[2];
  entry.password = fields[3];
  entry.url = fields[4];
  entry.notes = fields[5];
  entry.category = fields[6];
  entry.created = std::stoll(fields[7]);
  entry.last_modified = std::stoll(fields[8]);
  return entry;
}
