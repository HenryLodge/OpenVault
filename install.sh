#!/bin/bash

# OpenVault Installation Script
# Installs dependencies, builds, and installs to /usr/local/bin

set -e  # Exit on error

echo "=========================================="
echo "OpenVault Installation"
echo "=========================================="
echo ""

# Check if running from project directory
if [ ! -f "Makefile" ]; then
    echo "✗ Error: Must run from OpenVault project directory"
    exit 1
fi

# Step 1: Check dependencies
echo "Checking dependencies..."

NEEDS_SETUP=false

# Check for OpenSSL development libraries (needed for compilation)
if pkg-config --exists openssl 2>/dev/null || [ -f "/usr/include/openssl/ssl.h" ]; then
    echo "✓ OpenSSL development libraries found"
else
    echo "⚠ OpenSSL development libraries not found"
    NEEDS_SETUP=true
fi

if [ ! -d "extern/cxxtest-4.4" ]; then
    echo "⚠ CxxTest not found"
    NEEDS_SETUP=true
fi

if [ "$NEEDS_SETUP" = true ]; then
    echo ""
    echo "Missing dependencies detected."
    read -p "Run SETUP.sh to install dependencies? [Y/n] " -n 1 -r
    echo ""
    
    if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
        echo ""
        echo "Running SETUP.sh..."
        echo "=========================================="
        chmod +x SETUP.sh
        ./SETUP.sh
        
        if [ $? -ne 0 ]; then
            echo "✗ Error: Dependency installation failed"
            exit 1
        fi
    else
        echo ""
        echo "Skipping dependency installation."
        echo "⚠ Build may fail without dependencies!"
        read -p "Continue anyway? [y/N] " -n 1 -r
        echo ""
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "Installation cancelled."
            exit 0
        fi
    fi
else
    echo "✓ All dependencies found"
fi

echo ""

# Step 2: Build release version
echo "Building OpenVault (release version)..."
echo "=========================================="
make clean
make exe

if [ ! -f "bin/main" ]; then
    echo "✗ Error: Build failed - bin/main not found"
    exit 1
fi

echo "✓ Build successful"
echo ""

# Step 3: Install to /usr/local/bin
echo "Installing to /usr/local/bin..."
echo "=========================================="
echo "(This requires sudo privileges)"
echo ""

sudo cp bin/main /usr/local/bin/openvault
sudo chmod +x /usr/local/bin/openvault

# Verify installation
if command -v openvault &> /dev/null; then
    INSTALLED_VERSION=$(openvault --version 2>&1 | head -n1)
    echo "✓ OpenVault installed successfully!"
    echo "  Location: /usr/local/bin/openvault"
    echo "  Version: $INSTALLED_VERSION"
else
    echo "✗ Warning: openvault command not found in PATH"
    echo "You may need to add /usr/local/bin to your PATH"
fi

echo ""
echo "=========================================="
echo "Installation Complete"
echo "=========================================="
echo ""
echo "OpenVault is now installed and ready to use!"
echo ""
echo "Quick Start:"
echo "  openvault ~/passwords.ovault create"
echo "  openvault ~/passwords.ovault add-password"
echo "  openvault ~/passwords.ovault list-passwords"
echo ""
echo "For full documentation:"
echo "  openvault --help"
echo "  cat README.md"
echo ""
echo "To uninstall:"
echo "  ./uninstall.sh"
echo ""