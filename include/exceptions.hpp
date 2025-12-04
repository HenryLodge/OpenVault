#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

class CustomException : public std::exception {
  private:
    std::string exception_message;

  public:
    explicit CustomException(const std::string &msg) : exception_message(msg) {}
    virtual ~CustomException() noexcept {}

    virtual const char *what() const noexcept override
    {
      return exception_message.c_str();
    }
};

// exception for invalid entry
class EntryException : public CustomException {
  public:
    explicit EntryException(const std::string &msg) : CustomException("Entry Error: " + msg) {}
};

// exception for cryptography function errors
class CryptographyException : public CustomException {
  public:
    explicit CryptoException(const std::string &msg) : CustomException("Cryptography Error: " + msg) {}
};

// exception for file errors
class FileException : public CustomException {
  public:
    explicit FileException(const std::string &msg) : CustomException("File Error: " + msg) {}
};

// exception for invalid password
class InvalidPasswordException : public CustomException {
  public:
    explicit InvalidPasswordException(const std::string &msg = "Invalid password") : CustomException(msg) {}
};

// exception for a corrupted vault
class CorruptedVaultException : public CustomException {
  public:
    explicit CorruptedVaultException(const std::string &msg = "Vault data is corrupt") : CustomException(msg) {}
};

#endif
