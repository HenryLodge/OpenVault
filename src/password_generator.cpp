#include "password_generator.hpp"
#include <random>
#include <algorithm>
#include <stdexcept>

// all possible chars
const std::string PasswordGenerator::LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
const std::string PasswordGenerator::UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string PasswordGenerator::DIGITS = "0123456789";
const std::string PasswordGenerator::SYMBOLS = "!@#$%^&*()_+-=[]{}|;:,.<>?";

// default generate
std::string PasswordGenerator::generate(int length) {
  return generate(length, true, true, true, true);
}

// generate with charsets
std::string PasswordGenerator::generate(int length, bool useLowercase, bool useUppercase, bool useDigits, bool useSymbols) {
  std::string character_set = getCharacterSet(useLowercase, useUppercase, useDigits, useSymbols);

  if (character_set.empty()) {
    throw std::invalid_argument("At least one character set must be enabled to generate a password");
  }

  if (length < 4) {
    throw std::invalid_argument("Length of password must be at least 4 characters");
  }

  // random select char by char
  std::random_device rand;
  std::mt19937 gen(rand());
  std::uniform_int_distribution<> distribution(0, character_set.length() - 1);

  std::string password;
  password.reserve(length);

  for (int i = 0; i < length; ++i) {
    password += character_set[distribution(gen)];
  }

  return password;
}

// calculate password strength
int PasswordGenerator::calculateStrength(const std::string &password) {
  int strength = 0;
  int length = password.length();

  // ;ength score 10-40
  if (length >= 16) {
    strength += 40;
  }
  else if (length >= 12) {
    strength += 30;
  }
  else if (length >= 8) {
    strength += 20;
  }
  else {
    strength += 10;
  }

  // character score 10-40
  bool hasLower = false;
  bool hasUpper = false;
  bool hasDigit = false;
  bool hasSymbol = false;

  for (char c : password) {
    if (!hasLower || !hasUpper || !hasDigit || !hasSymbol) {
      if (std::islower(c)) {
        hasLower = true;
      }
      else if (std::isupper(c)) {
        hasUpper = true;
      }
      else if (std::isdigit(c)) {
        hasDigit = true;
      }
      else {
        hasSymbol = true;
      }
    } else {
      break;
    }
  }

  int varieties = hasLower + hasUpper + hasDigit + hasSymbol;
  strength += varieties * 10;

  // char unique score 10-40
  std::string sorted = password;
  std::sort(sorted.begin(), sorted.end());
  auto last = std::unique(sorted.begin(), sorted.end());
  int uniqueChars = std::distance(sorted.begin(), last);
  strength += std::min(20, (uniqueChars * 20) / length);

  return std::min(100, strength);
}

// strength description based on password strength
std::string PasswordGenerator::getStrengthDescription(int strength) {
  if (strength >= 80) {
    return "Very Strong";
  }
  if (strength >= 60) {
    return "Strong";
  }
  if (strength >= 40) {
    return "Average";
  }
  if (strength >= 20) {
    return "Weak";
  }
  return "Very Weak";
}

// get character set if bool true
std::string PasswordGenerator::getCharacterSet(bool useLowercase, bool useUppercase, bool useDigits, bool useSymbols) {
  std::string charset;
  if (useLowercase) {
    charset += LOWERCASE;
  }
  if (useUppercase) {
    charset += UPPERCASE;
  }
  if (useDigits) {
    charset += DIGITS;
  }
  if (useSymbols) {
    charset += SYMBOLS;
  }
  return charset;
}
