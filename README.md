# OpenVault

**Secure by design. Open by nature.**

OpenVault is an open-source, encrypted command-line password manager implementing AES-256-CBC encryption, PBKDF2 key derivation, and secure memory management that stores credentials in an encrypted vault file.

---

## Features

### Security
- **AES-256-CBC encryption** for all stored data
- **PBKDF2 key derivation** with 100,000 iterations
- **Unique salt** per vault prevents rainbow table attacks
- **Secure memory wiping** for sensitive data
- **Master password never stored** - only salted hash for verification

### Password Management
- Store unlimited password entries with metadata
- Organize with categories and tags
- Search passwords with case-insensitive matching
- Password strength analysis and recommendations
- Secure password generator with configurable options

### Vault Operations
- Create encrypted vaults protected by master password
- Change master password (re-encrypts all data)
- Export passwords to CSV (with security warnings)
- Multiple independent vaults supported
- Automatic backup during password changes

---

## Quick Start

### Installation

1. **Clone Repo:**
```bash
   git clone https://github.com/HenryLodge/OpenVault.git
```

2. **Navigate and Install:**
```bash
  cd OpenVault
  ./install.sh
  #  OPTIONAL WAY (does same as above)
  ./SETUP.sh
  ./install.sh
```

### Basic Usage

**Create a vault:**
```bash
openvault my_passwords.ovault create
# Enter master password when prompted
```

**Add a password:**
```bash
openvault my_passwords.ovault add-password
# Follow interactive prompts
```

**List all passwords:**
```bash
openvault my_passwords.ovault list-passwords
```

**View password details:**
```bash
openvault my_passwords.ovault get 1
```

---

## Commands

### Vault Management
- `create` - Create new encrypted vault in current directory
- `change-password` - Change master password and re-encrypt
- `info` - Display vault statistics
- `export <file.csv>` - Export passwords to CSV (unencrypted)

### Password Operations
- `add-password` - Add new password entry
- `list-passwords` - List all entries with strength indicators
- `get <id>` - Show detailed password information
- `edit <id>` - Modify existing password entry
- `delete <id>` - Delete password entry (with confirmation)
- `search <query>` - Search across service, username, category

### Utilities
- `generate [length]` - Generate secure password
- `--help` - Show usage information
- `--version` - Show version information

---

## Command Reference Table

| Command | Arguments | Description | Example |
|---------|-----------|-------------|---------|
| `create` | None | Create new encrypted vault | `openvault my.ovault create` |
| `add-password` | None | Add new password entry (interactive prompts) | `openvault my.ovault add-password` |
| `list-passwords` | None | List all password entries with strength | `openvault my.ovault list-passwords` |
| `get` | `<id>` | Show detailed password information | `openvault my.ovault get 1` |
| `search` | `<query>` | Search passwords by service/username/category | `openvault my.ovault search github` |
| `edit` | `<id>` | Modify existing password entry | `openvault my.ovault edit 1` |
| `delete` | `<id>` | Delete password entry with confirmation | `openvault my.ovault delete 1` |
| `generate` | `[length]` | Generate secure random password | `openvault my.ovault generate 24` |
| `info` | None | Display vault statistics and categories | `openvault my.ovault info` |
| `change-password` | None | Change master password and re-encrypt vault | `openvault my.ovault change-password` |
| `export` | `<output.csv>` | Export passwords to CSV (unencrypted) | `openvault my.ovault export backup.csv` |
| `--help` | None | Show usage information | `openvault --help` |
| `--version` | None | Show version information | `openvault --version` |

---

<!-- --- -->

## Vault File Format

OpenVault uses a custom binary format:
```
[HEADER - 128 bytes]
├── Magic number: "OVLT" (4 bytes)
├── Version: 1 (4 bytes)
├── Salt: random (16 bytes)
├── Iterations: 100000 (4 bytes)
├── Password hash: SHA-256 (32 bytes)
└── Reserved: (68 bytes)

[ENCRYPTED DATA]
├── Entry count (encrypted)
└── Password entries (each encrypted separately)
```

**File extension:** `.ovault` (OpenVault file)

---

## Security Notes

### What OpenVault Does
- Encrypts all passwords with AES-256-CBC  
- Uses strong key derivation (PBKDF2, 100k iterations)  
- Generates cryptographically secure passwords  
- Wipes sensitive data from memory  
- Never stores master password  

### What OpenVault Doesn't Do
- No cloud sync (local files only)  
- No two-factor authentication  
- No password breach checking  
- No clipboard auto-clear  

---

## Development

### Contributing
OpenVault is open source under the MIT License. Contributions, issues, and feature requests are welcome.

### Future Plans
- Upgrade to AES-256-GCM for authenticated encryption
- Document encryption support
- Password history tracking
- GUI interface
- Password breach checking 

---

## License

MIT License - Copyright (c) 2025 Henry Lodge

---

## Author

**Henry Lodge** | CS @ Northeastern University, Class of 2028

**Contact:**
 - GitHub: https://github.com/HenryLodge
 - LinkedIn: https://www.linkedin.com/in/henrylodge/
 - Email: hlcodes01@gmail.com