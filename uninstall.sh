#!/bin/bash

# OpenVault Uninstallation Script

echo "=========================================="
echo "OpenVault Uninstallation"
echo "=========================================="
echo ""

if [ ! -f "/usr/local/bin/openvault" ]; then
    echo "OpenVault is not installed in /usr/local/bin"
    echo ""
    echo "Checking other common locations..."
    
    if [ -f "/usr/bin/openvault" ]; then
        echo "Found in /usr/bin/openvault"
        sudo rm /usr/bin/openvault
        echo "✓ Removed from /usr/bin"
    else
        echo "OpenVault not found in standard locations"
        exit 1
    fi
else
    echo "Removing OpenVault from /usr/local/bin..."
    sudo rm /usr/local/bin/openvault
    echo "✓ OpenVault uninstalled"
fi

echo ""
echo "=========================================="
echo "Uninstallation Complete"
echo "=========================================="
echo ""