#ifndef PASSWORD_GENERATOR_HPP
#define PASSWORD_GENERATOR_HPP

#include <string>
#include <vector>

class PasswordGenerator {
private:
  static const std::string LOWERCASE;
  static const std::string UPPERCASE;
  static const std::string DIGITS;
  static const std::string SYMBOLS;

public:
  // gen password, default
  static std::string generate(int length = 16);

  // gen password custom character fields
  static std::string generate(int length, bool useLowercase, bool useUppercase, bool useDigits, bool useSymbols);

  // calculate password strength
  static int calculateStrength(const std::string &password);

  // get description of strength based on calculateStrength() return
  static std::string getStrengthDescription(int strength);

private:
  // getter
  static std::string getCharacterSet(bool useLowercase, bool useUppercase, bool useDigits, bool useSymbols);
};

#endif
