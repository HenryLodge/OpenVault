#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream> 
#include "vault.hpp"
#include "password_entry.hpp"
#include "password_generator.hpp"
#include "cli.hpp"
#include "exceptions.hpp"

// handle list password command input
void handleListPasswords(const std::string& vaultFile) {
  std::string master_password = CLI::readPassword("Master password: ");
  
  Vault vault(vaultFile);
  vault.open(master_password);
  
  auto entries = vault.getAllEntries();
  
  // sort
  std::sort(entries.begin(), entries.end(), [](const PasswordEntry& a, const PasswordEntry& b) {
    return a.getService() < b.getService();
  });
  
  CLI::displayPasswordTable(entries);
}

// handle search command input
void handleSearch(const std::string& vaultFile, const std::string& query) {
  std::string master_password = CLI::readPassword("Master password: ");
  
  Vault vault(vaultFile);
  vault.open(master_password);
  
  auto entries = vault.getAllEntries();
  std::vector<PasswordEntry> results;
  
  // filter
  std::copy_if(entries.begin(), entries.end(), std::back_inserter(results), [&query](const PasswordEntry& entry) {
    std::string low_query = query;
    std::transform(low_query.begin(), low_query.end(), low_query.begin(), ::tolower);
    
    std::string service = entry.getService();
    std::string username = entry.getUsername();
    std::string category = entry.getCategory();
    
    std::transform(service.begin(), service.end(), service.begin(), ::tolower);
    std::transform(username.begin(), username.end(), username.begin(), ::tolower);
    std::transform(category.begin(), category.end(), category.begin(), ::tolower);
    
    return (service.find(low_query) != (std::string::npos || username.find(low_query) != std::string::npos || category.find(low_query) != std::string::npos));
  });
  
  if (results.empty()) {
    CLI::printInfo("No entries found matching: " + query);
  } else {
    CLI::printSuccess("Found " + std::to_string(results.size()) + " matching entries:");
    CLI::displayPasswordTable(results);
  }
}

// handle create vault command input
void handleCreate(const std::string& vaultFile) {
  std::cout << "Creating new vault: " << vaultFile << "\n";
  
  std::string password1 = CLI::readPassword("Enter master password: ");
  std::string password2 = CLI::readPassword("Confirm master password: ");
  
  // check pass reqs
  if (password1 != password2) {
    CLI::printError("Passwords do not match");
    return;
  }
  if (password1.length() < 8) {
    CLI::printError("Master password must be at least 8 characters");
    return;
  }
  
  // create vault
  Vault vault(vaultFile);
  vault.create(password1);
  
  CLI::printSuccess("Vault created successfully");
  CLI::printInfo("You can now add password entries.");
}

// handle add entry command input
void handleAddPassword(const std::string& vaultFile) {
  std::string master_password = CLI::readPassword("Master password: ");
  
  Vault vault(vaultFile);
  vault.open(master_password);
  
  // get info
  std::cout << "\n";
  CLI::printSeparator('=', 50);
  std::cout << "Add New Password Entry\n";
  CLI::printSeparator('=', 50);
  
  std::string service = CLI::readLine("Service/Website: ");
  std::string username = CLI::readLine("Username: ");
  
  std::string password;
  std::string password_choice = CLI::readLine("Password (leave empty to generate): ");
  
  if (password_choice.empty()) {
    bool generate = CLI::readYesNo("Generate password?");
    if (generate) {
      std::string lengthStr = CLI::readLine("Length [16]: ");
      int length = lengthStr.empty() ? 16 : std::stoi(lengthStr);
      
      bool useUpper = CLI::readYesNo("Include uppercase?");
      bool useDigits = CLI::readYesNo("Include numbers?");
      bool useSymbols = CLI::readYesNo("Include symbols?");
      
      password = PasswordGenerator::generate(length, true, useUpper, 
                                            useDigits, useSymbols);
      std::cout << "\nGenerated password: " << password << "\n\n";
    } else {
      password = CLI::readPassword("Password: ");
    }
  } else {
    password = password_choice;
  }
  
  std::string url = CLI::readLine("URL (optional): ");
  std::string category = CLI::readLine("Category (optional): ");
  std::string notes = CLI::readLine("Notes (optional): ");
  
  // create obj
  PasswordEntry entry(0, service, username, password);
  entry.setUrl(url);
  entry.setCategory(category);
  entry.setNotes(notes);
  
  // add to vault
  int id = vault.addEntry(entry);
  
  CLI::printSuccess("Password entry added (ID: " + std::to_string(id) + ")");
}

// handle get entry get command input
void handleGet(const std::string& vaultFile, int id) {
  std::string master_password = CLI::readPassword("Master password: ");
  
  Vault vault(vaultFile);
  vault.open(master_password);
  
  PasswordEntry entry = vault.getEntry(id);
  CLI::displayPasswordDetail(entry);
}

// hangle entry delete command input
void handleDelete(const std::string& vaultFile, int id) {
  std::string master_password = CLI::readPassword("Master password: ");
  
  Vault vault(vaultFile);
  vault.open(master_password);
  
  PasswordEntry entry = vault.getEntry(id);
  
  std::cout << "Delete password entry for: " << entry.getService() << " (" << entry.getUsername() << ")\n";
  
  // user confirm
  bool confirm = CLI::readYesNo("Are you sure?");
  
  if (confirm) {
    vault.deleteEntry(id);
    CLI::printSuccess("Password entry deleted");
  } else {
    CLI::printInfo("Cancelled");
  }
}

// handle entry edit command input
void handleEdit(const std::string& vaultFile, int id) {
  std::string master_password = CLI::readPassword("Master password: ");
  
  Vault vault(vaultFile);
  vault.open(master_password);
  
  PasswordEntry entry = vault.getEntry(id);
  
  std::cout << "\nEditing entry: " << entry.getService() << "\n";
  std::cout << "(Leave empty to keep current value)\n\n";
  
  std::string service = CLI::readLine("Service [" + entry.getService() + "]: ");
  std::string username = CLI::readLine("Username [" + entry.getUsername() + "]: ");
  std::string password = CLI::readPassword("Password [unchanged]: ");
  std::string url = CLI::readLine("URL [" + entry.getUrl() + "]: ");
  std::string category = CLI::readLine("Category [" + entry.getCategory() + "]: ");
  std::string notes = CLI::readLine("Notes [" + entry.getNotes() + "]: ");
  
  // set values
  if (!service.empty()) {
    entry.setService(service);
  }
  if (!username.empty()) {
    entry.setUsername(username);
  }
  if (!password.empty()) {
    entry.setPassword(password);
  }
  if (!url.empty()) {
    entry.setUrl(url);
  }
  if (!category.empty()) {
    entry.setCategory(category);
  }
  if (!notes.empty()) {
    entry.setNotes(notes);
  }
  
  // save
  vault.updateEntry(entry);
  
  CLI::printSuccess("Password entry updated");
}

// handle password generate command input
void handleGenerate(int length) {
  std::string password = PasswordGenerator::generate(length);
  int strength = PasswordGenerator::calculateStrength(password);
  
  std::cout << "\nGenerated password: " << password << "\n";
  std::cout << "Strength: " << strength << "/100 (" << PasswordGenerator::getStrengthDescription(strength) << ")\n";
  std::cout << "Length: " << password.length() << " characters\n\n";
}

// handle vault info command input
void handleInfo(const std::string& vaultFile) {
  std::string master_password = CLI::readPassword("Master password: ");
  
  Vault vault(vaultFile);
  vault.open(master_password);
  
  auto entries = vault.getAllEntries();
  
  std::cout << "\n";
  CLI::printSeparator('=', 50);
  std::cout << "Vault Information\n";
  CLI::printSeparator('=', 50);
  std::cout << "File:    " << vaultFile << "\n";
  std::cout << "Entries: " << entries.size() << "\n";
  
  // count categories
  std::map<std::string, int> count;
  std::for_each(entries.begin(), entries.end(), [&count](const PasswordEntry& entry) {
    std::string category = entry.getCategory();
    if (category.empty()) {
      category = "(none)";
    }
    count[category]++;
  });
  
  if (!count.empty()) {
    std::cout << "\nCategories:\n";
    for (const auto& cat : count) {
      std::cout << "  " << cat.first << ": " << cat.second << "\n";
    }
  }
  
  CLI::printSeparator('=', 50);
  std::cout << "\n";
}

// handle change vault password command input
void handleChangePassword(const std::string& vaultFile) {
  std::string old_password = CLI::readPassword("Current master password: ");
  
  Vault vault(vaultFile);
  vault.open(old_password);
  
  std::cout << "\n";
  
  // new pass
  std::string new_password1 = CLI::readPassword("New master password: ");
  std::string new_password2 = CLI::readPassword("Confirm new password: ");
  
  if (new_password1 != new_password2) {
    CLI::printError("Passwords do not match");
    return;
  }
  
  if (new_password1.length() < 8) {
    CLI::printError("Password must be at least 8 characters");
    return;
  }
  
  if (new_password1 == old_password) {
    CLI::printError("New password must be different from old password");
    return;
  }
  
  auto entries = vault.getAllEntries();
  vault.close();
  
  // make temp backup
  std::string backupFile = vaultFile + ".backup";
  std::ifstream src(vaultFile, std::ios::binary);
  std::ofstream dst(backupFile, std::ios::binary);
  dst << src.rdbuf();
  src.close();
  dst.close();
  
  CLI::printInfo("Created backup: " + backupFile);
  
  try {
    std::remove(vaultFile.c_str());
    Vault newVault(vaultFile);
    newVault.create(new_password1);
    
    CLI::printInfo("Re-encrypting " + std::to_string(entries.size()) + " entries...");
    
    for (const auto& entry : entries) {
      newVault.addEntry(entry);
    }
    
    newVault.close();
    std::remove(backupFile.c_str());
    
    CLI::printSuccess("Master password changed successfully");
    CLI::printInfo("All entries re-encrypted with new password");
  } catch (...) {
    std::remove(vaultFile.c_str());
    std::rename(backupFile.c_str(), vaultFile.c_str());
    CLI::printError("Failed to change password. Vault restored from backup.");
    throw;
  }
}

// handle vault export command input
void handleExport(const std::string& vaultFile, const std::string& outputFile) {
  std::string master_password = CLI::readPassword("Master password: ");
  
  Vault vault(vaultFile);
  vault.open(master_password);
  
  auto entries = vault.getAllEntries();
  
  std::ofstream out(outputFile);
  if (!out) {
    CLI::printError("Cannot create output file: " + outputFile);
    return;
  }
  
  // export csv
  out << "Service,Username,Password,URL,Category,Notes\n";
  for (const auto& entry : entries) {
    out << "\"" << entry.getService() << "\","
        << "\"" << entry.getUsername() << "\","
        << "\"" << entry.getPassword() << "\","
        << "\"" << entry.getUrl() << "\","
        << "\"" << entry.getCategory() << "\","
        << "\"" << entry.getNotes() << "\"\n";
  }
  out.close();
  
  CLI::printSuccess("Exported " + std::to_string(entries.size()) + " entries to: " + outputFile);
  std::cout << "WARNING: Exported file contains UNENCRYPTED passwords\n";
  CLI::printInfo("Delete the file after use or encrypt it separately");
}

int main(int argc, char* argv[]) {
  try {
    // flags
    if (argc == 2) {
      std::string arg = argv[1];
      if (arg == "--help" || arg == "-h") {
        CLI::showUsage();
        return 0;
      }
      if (arg == "--version" || arg == "-v") {
        CLI::showVersion();
        return 0;
      }
    }
    
    // min cli fields
    if (argc < 3) {
      CLI::showUsage();
      return 1;
    }
    
    std::string vault_file = argv[1];
    std::string command = argv[2];
    
    // check command
    if (command == "create") {
      handleCreate(vault_file);
    } else if (command == "add-password") {
      handleAddPassword(vault_file);
    } else if (command == "list-passwords" || command == "list") {
      handleListPasswords(vault_file);
    } else if (command == "search") {
      if (argc < 4) {
        CLI::printError("Usage: openvault <vault> search <query>");
        return 1;
      }
      handleSearch(vault_file, argv[3]);
    } else if (command == "get") {
      if (argc < 4) {
        CLI::printError("Usage: openvault <vault> get <id>");
        return 1;
      }
      handleGet(vault_file, std::stoi(argv[3]));
    } else if (command == "edit") {
      if (argc < 4) {
        CLI::printError("Usage: openvault <vault> edit <id>");
        return 1;
      }
      handleEdit(vault_file, std::stoi(argv[3]));
    } else if (command == "delete") {
      if (argc < 4) {
        CLI::printError("Usage: openvault <vault> delete <id>");
        return 1;
      }
      handleDelete(vault_file, std::stoi(argv[3]));
    } else if (command == "generate") {
      int length = (argc >= 4) ? std::stoi(argv[3]) : 16;
      handleGenerate(length);
    } else if (command == "info") {
      handleInfo(vault_file);
    } else if (command == "change-password") {
      handleChangePassword(vault_file);
    } else if (command == "export") {
      if (argc < 4) {
        CLI::printError("Usage: openvault <vault> export <output.csv>");
        return 1;
      }
      handleExport(vault_file, argv[3]);
    } else {
      CLI::printError("Unknown command: " + command);
      CLI::printInfo("Use 'openvault --help' for usage information");
      return 1;
    }
    
    return 0;
  } catch (const InvalidPasswordException& e) {
      CLI::printError(e.what());
      return 1;
  } catch (const FileException& e) {
      CLI::printError(e.what());
      return 1;
  } catch (const CustomException& e) {
      CLI::printError(e.what());
      return 1;
  } catch (const std::exception& e) {
      CLI::printError(std::string("Unexpected error: ") + e.what());
      return 1;
  }
}