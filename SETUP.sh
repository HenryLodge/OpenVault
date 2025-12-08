#!/bin/bash

# SETUP.sh - Install dependencies for OpenVault
# Run this script once before building: chmod +x SETUP.sh && ./SETUP.sh

set -e  # Exit on error

echo "=========================================="
echo "OpenVault - Dependency Setup"
echo "=========================================="
echo ""

# Detect operating system
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Detected: Linux"
    echo "Installing required packages..."
    
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y libssl-dev valgrind g++ make curl unzip python3 python-is-python3
        echo "✓ Packages installed successfully"
    elif command -v yum &> /dev/null; then
        sudo yum install -y openssl-devel valgrind gcc-c++ make curl unzip python3
        echo "✓ Packages installed successfully"
        
        # Create python symlink for yum-based systems
        if ! command -v python &> /dev/null; then
            sudo ln -sf $(which python3) /usr/bin/python
            echo "✓ Created python -> python3 symlink"
        fi
    elif command -v pacman &> /dev/null; then
        sudo pacman -S --noconfirm openssl valgrind gcc make curl unzip python
        echo "✓ Packages installed successfully"
    else
        echo "✗ Could not detect package manager"
        exit 1
    fi

elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Detected: macOS"
    
    if command -v brew &> /dev/null; then
        echo "Installing required packages via Homebrew..."
        brew install openssl valgrind curl python3
        echo "✓ Packages installed successfully"
        
        # Create python symlink if needed
        if ! command -v python &> /dev/null; then
            sudo ln -sf $(which python3) /usr/local/bin/python
            echo "✓ Created python -> python3 symlink"
        fi
    else
        echo "✗ Homebrew not found"
        echo "Please install Homebrew first: https://brew.sh"
        exit 1
    fi

else
    echo "✗ Unknown operating system: $OSTYPE"
    exit 1
fi

# Verify Python is available
echo ""
echo "Checking Python..."
if command -v python &> /dev/null; then
    echo "✓ Python: $(python --version 2>&1)"
elif command -v python3 &> /dev/null; then
    echo "⚠ python3 found but 'python' command missing"
    echo "Creating symlink..."
    
    # Try to create symlink
    if sudo ln -sf $(which python3) /usr/local/bin/python 2>/dev/null; then
        echo "✓ Created python -> python3 symlink"
    else
        echo "✗ Could not create symlink. Will modify CxxTest instead."
        NEED_CXXTEST_FIX=true
    fi
else
    echo "✗ Python not found. Please install Python 3."
    exit 1
fi

echo ""
echo "=========================================="
echo "Installing CxxTest..."
echo "=========================================="

# Create cxxtest directory
mkdir -p cxxtest

# Download CxxTest if not already present
if [ ! -d "cxxtest/cxxtest-4.4" ]; then
    echo "Downloading CxxTest from GitHub..."
    
    curl -o cxxtest.zip -L https://github.com/CxxTest/cxxtest/archive/a19f85fdf90f97e16d6e3e7e3d2d68c31cd89e3c.zip
    unzip cxxtest.zip
    mv cxxtest-a19f85fdf90f97e16d6e3e7e3d2d68c31cd89e3c cxxtest/cxxtest-4.4
    rm cxxtest.zip
    chmod +x cxxtest/cxxtest-4.4/bin/cxxtestgen
    
    echo "✓ CxxTest downloaded"
else
    echo "✓ CxxTest already present"
fi

# Fix CxxTest to use python3 if needed
if [ "$NEED_CXXTEST_FIX" = true ]; then
    echo "Fixing CxxTest to use python3..."
    sed -i.bak '1s|^#!/usr/bin/env python$|#!/usr/bin/env python3|' cxxtest/cxxtest-4.4/bin/cxxtestgen
    echo "✓ CxxTest fixed to use python3"
fi

# Verify CxxTest works
if [ -f "cxxtest/cxxtest-4.4/bin/cxxtestgen" ]; then
    echo "✓ CxxTest installed: cxxtest/cxxtest-4.4/bin/cxxtestgen"
    
    # Test if it runs
    if cxxtest/cxxtest-4.4/bin/cxxtestgen --version &>/dev/null; then
        echo "✓ CxxTest is working"
    else
        echo "⚠ CxxTest may have issues. Trying to fix..."
        sed -i.bak '1s|^#!/usr/bin/env python$|#!/usr/bin/env python3|' cxxtest/cxxtest-4.4/bin/cxxtestgen
    fi
else
    echo "✗ CxxTest installation failed"
    exit 1
fi

echo ""
echo "=========================================="
echo "Verifying installations..."
echo "=========================================="

# Verify OpenSSL
if command -v openssl &> /dev/null; then
    echo "✓ OpenSSL: $(openssl version)"
else
    echo "✗ OpenSSL not found"
    exit 1
fi

# Verify g++
if command -v g++ &> /dev/null; then
    echo "✓ g++: $(g++ --version | head -n1)"
else
    echo "✗ g++ not found"
    exit 1
fi

# Verify make
if command -v make &> /dev/null; then
    echo "✓ make: $(make --version | head -n1)"
else
    echo "✗ make not found"
    exit 1
fi

# Verify Python
if command -v python &> /dev/null; then
    echo "✓ python: $(python --version 2>&1)"
elif command -v python3 &> /dev/null; then
    echo "✓ python3: $(python3 --version)"
else
    echo "✗ Python not found"
    exit 1
fi

# Verify valgrind
if command -v valgrind &> /dev/null; then
    echo "✓ valgrind: $(valgrind --version)"
else
    echo "⚠ valgrind not found (optional)"
fi

echo ""
echo "=========================================="
echo "Setup Complete!"
echo "=========================================="
echo ""
echo "Directory structure:"
echo "  cxxtest/cxxtest-4.4/     - CxxTest framework"
echo "  include/                - Header files"
echo "  src/                    - Source files"
echo "  test/                   - Unit tests"
echo ""
echo "Next steps:"
echo "  1. Build the project:"
echo "     make clean"
echo "     make tests_dbg"
echo "  2. Run tests:"
echo "     make run-tests_dbg"
echo "  3. Run with valgrind:"
echo "     valgrind --leak-check=full ./bin/test_dbg"
echo ""