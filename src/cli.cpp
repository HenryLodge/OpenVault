#include "cli.hpp"
#include "password_entry.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <termios.h>
#include <unistd.h>
#include <algorithm>

namespace CLI {
  // print usage commands and info to cli
  void showUsage() {
    std::cout << "OpenVault - Encrypted Password Manager\n";
    std::cout << "Usage: openvault <vault_file> <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  create                Create new vault\n";
    std::cout << "  add-password          Add password entry\n";
    std::cout << "  list-passwords        List all passwords\n";
    std::cout << "  get <id>              Show password details\n";
    std::cout << "  search <query>        Search passwords\n";
    std::cout << "  edit <id>             Edit password entry\n";
    std::cout << "  delete <id>           Delete password entry\n";
    std::cout << "  generate [length]     Generate secure password\n";
    std::cout << "  info                  Show vault statistics\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --help                Show this help\n";
    std::cout << "  --version             Show version\n";
    std::cout << "\nExamples:\n";
    std::cout << "  openvault my.ovault create\n";
    std::cout << "  openvault my.ovault add-password\n";
    std::cout << "  openvault my.ovault search github\n";
  }

  // print version to cli
  void showVersion() {
    std::cout << "OpenVault version 1.0\n";
    std::cout << "AES-256 encrypted command-line password manager\n";
    std::cout << "Support for documents coming soon!\n";
  }

  // print help info to cli
  void showHelp(const std::string &command) {
    if (command.empty()) {
      showUsage();
      return;
    }

    std::cout << "Help for command: " << command << "\n\n";

    if (command == "create") {
      std::cout << "Create a new encrypted vault.\n";
      std::cout << "Usage: openvault <vault_file> create\n";
    }
    else if (command == "add-password") {
      std::cout << "Add a new password entry to the vault.\n";
      std::cout << "Usage: openvault <vault_file> add-password\n";
    }
    else if (command == "list-passwords") {
      std::cout << "List all password entries in the vault.\n";
      std::cout << "Usage: openvault <vault_file> list-passwords\n";
    }
    else {
      std::cout << "No detailed help available for: " << command << "\n";
      std::cout << "Use 'openvault --help' for general help.\n";
    }
  }

  // read password from user
  std::string readPassword(const std::string &prompt) {
    std::cout << prompt;
    std::cout.flush();

    // turn off echo
    termios oldSettings;
    termios newSettings;
    tcgetattr(STDIN_FILENO, &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

    // store
    std::string password;
    std::getline(std::cin, password);

    // turn on echo
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    std::cout << std::endl;

    return password;
  }

  // read line
  std::string readLine(const std::string &prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
  }

  bool readYesNo(const std::string &prompt) {
    std::string response = readLine(prompt + " [y/n]: ");
    return !response.empty() && (response[0] == 'y' || response[0] == 'Y');
  }

  // print table of all entries in vault
  void displayPasswordTable(const std::vector<::PasswordEntry> &entries) {
    if (entries.empty()) {
      std::cout << "No password entries found.\n";
      return;
    }

    // header info
    std::cout << "\n";
    printSeparator('-', 80);
    std::cout << std::left << std::setw(5) << "ID" << std::setw(20) << "Service" << std::setw(25) << "Username"
              << std::setw(15) << "Category" << std::setw(15) << "Modified" << "\n";
    printSeparator('-', 80);

    // entries
    for (const auto &entry : entries) {
      std::cout << std::left << std::setw(5) << entry.getId() << std::setw(20) << entry.getService().substr(0, 19)
                << std::setw(25) << entry.getUsername().substr(0, 24) << std::setw(15) << entry.getCategory().substr(0, 14)
                << std::setw(15) << formatDate(entry.getModified()) << "\n";
    }
    printSeparator('-', 80);
    std::cout << "Total: " << entries.size() << " entries\n\n";
  }

  // print all entry info to cli
  void displayPasswordDetail(const ::PasswordEntry &entry) {
    std::cout << "\n";
    printSeparator('=', 50);
    std::cout << "Password Entry #" << entry.getId() << "\n";
    printSeparator('=', 50);
    std::cout << "Service:   " << entry.getService() << "\n";
    std::cout << "Username:  " << entry.getUsername() << "\n";
    std::cout << "Password:  " << entry.getPassword() << "\n";

    if (!entry.getUrl().empty()) {
      std::cout << "URL:       " << entry.getUrl() << "\n";
    }
    if (!entry.getCategory().empty()) {
      std::cout << "Category:  " << entry.getCategory() << "\n";
    }
    if (!entry.getNotes().empty()) {
      std::cout << "Notes:     " << entry.getNotes() << "\n";
    }

    std::cout << "\nCreated:   " << formatDate(entry.getCreated()) << "\n";
    std::cout << "Modified:  " << formatDate(entry.getModified()) << "\n";
    printSeparator('=', 50);
    std::cout << "\n";
  }

  // format timestring
  std::string formatDate(time_t timestamp) {
    char buffer[20];
    struct tm *timeinfo = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", timeinfo);
    return std::string(buffer);
  }

  // helper to print line of chars
  void printSeparator(char ch, int width) {
    std::cout << std::string(width, ch) << "\n";
  }

  // print message with error title
  void printError(const std::string &message) {
    std::cerr << "Error: " << message << "\n";
  }

  // print message with Success title
  void printSuccess(const std::string &message) {
    std::cout << "Success: " << message << "\n";
  }

  // print message with dash in front
  void printInfo(const std::string &message) {
    std::cout << "- " << message << "\n";
  }
}
