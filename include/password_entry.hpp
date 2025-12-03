#ifndef PASSWORD_ENTRY_HPP
#define PASSWORD_ENTRY_HPP

#include <string>
#include <ctime>
#include <iostream>

class PasswordEntry {
private:
  int id;
  std::string service;
  std::string username;
  std::string password;
  std::string url;
  std::string notes;
  std::string category;
  time_t created;
  time_t last_modified;

public:
  // construct
  PasswordEntry();
  PasswordEntry(int id, const std::string& service, const std::string& username, const std::string& password);
  PasswordEntry(const PasswordEntry& other);
  
  // destruct
  ~PasswordEntry();
  
  // getters
  int getId() const {
    return id;
  }
  std::string getService() const {
    return service;
  }
  std::string getUsername() const {
    return username;
  }
  std::string getPassword() const {
    return password;
  }
  std::string getUrl() const {
    return url;
  }
  std::string getNotes() const {
    return notes;
  }
  std::string getCategory() const {
    return category;
  }
  time_t getCreated() const {
    return created;
  }
  time_t getModified() const {
    return last_modified;
  }
  
  // setters
  void setId(int newId) {
    id = newId;
    updateModified();
  }
  void setService(const std::string& s) {
    service = s;
    updateModified();
  }
  void setUsername(const std::string& u) {
    username = u;
    updateModified();
  }
  void setPassword(const std::string& p) {
    password = p;
    updateModified();
  }
  void setUrl(const std::string& u) {
    url = u;
    updateModified();
  }
  void setNotes(const std::string& n) {
    notes = n;
    updateModified();
  }
  void setCategory(const std::string& c) {
    category = c;
    updateModified();
  }
  
  // = operator
  PasswordEntry& operator=(const PasswordEntry& other);
  
  // == operator
  bool operator==(const PasswordEntry& other) const;
  
  // < operator
  bool operator<(const PasswordEntry& other) const;
  
  // << operator
  friend std::ostream& operator<<(std::ostream& os, const PasswordEntry& entry);
  
  // helpers
  void updateModified();
  std::string toString() const;
  
  // serials
  std::string serialize() const;
  static PasswordEntry deserialize(const std::string& data);
};

#endif
