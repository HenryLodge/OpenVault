#ifndef CLI_HPP
#define CLI_HPP

#include <string>
#include <vector>

namespace CLI {
  // cli output
  void showUsage();
  void showVersion();
  void showHelp(const std::string &command = "");

  // cli input
  std::string readPassword(const std::string &prompt);
  std::string readLine(const std::string &prompt);
  bool readYesNo(const std::string &prompt);

  // cli output full format
  void displayPasswordTable(const std::vector<class PasswordEntry> &entries);
  void displayPasswordDetail(const class PasswordEntry &entry);

  // helpers
  std::string formatDate(time_t timestamp);
  void clearScreen();
  void printSeparator(char ch = '=', int width = 50);
  void printError(const std::string &message);
  void printSuccess(const std::string &message);
  void printInfo(const std::string &message);
}

#endif
