#!/bin/bash

# OpenVault Installation Script
# Builds release version and installs to /usr/local/bin

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

# Build release version
echo "Building OpenVault (release)..."
make clean
make exe

if [ ! -f "bin/main" ]; then
    echo "✗ Error: Build failed - bin/main not found"
    exit 1
fi

echo "✓ Build successful"
echo ""

# Install to /usr/local/bin
echo "Installing to /usr/local/bin..."
echo "(This requires sudo privileges)"
echo ""

sudo cp bin/main /usr/local/bin/openvault
sudo chmod +x /usr/local/bin/openvault

echo "✓ OpenVault installed successfully!"
echo ""
echo "=========================================="
echo "Installation Complete"
echo "=========================================="
echo ""
echo "You can now use OpenVault from anywhere:"
echo "  openvault <vault_file> <command>"
echo ""
echo "Examples:"
echo "  openvault ~/passwords.ovault create"
echo "  openvault ~/passwords.ovault add-password"
echo "  openvault ~/passwords.ovault list-passwords"
echo ""
echo "To uninstall: ./uninstall.sh"
echo ""