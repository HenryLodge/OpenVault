#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

class CustomException : public std::exception {
private:
    std::string exception_message;
    
public:
  explicit CustomException(const std::string& msg) : exception_message(msg) {}
  virtual ~CustomException() noexcept {}
  
  virtual const char* what() const noexcept override {
    return exception_message.c_str();
  }
};

// exception for invalid entry
class EntryException : public CustomException {
public:
  explicit EntryException(const std::string& msg) : CustomException("Entry Error: " + msg) {}
};

#endif
