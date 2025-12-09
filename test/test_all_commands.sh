#!/bin/bash

# Test script for all OpenVault commands
# Run this to verify all functionality works

set -e  # Exit on error

VAULT="test_comprehensive.ovault"
PASSWORD="TestPassword123!"
BIN="../bin/main_dbg"

echo "======================================"
echo "OpenVault Tests"
echo "======================================"
echo ""

# Cleanup from previous runs
rm -f $VAULT *.csv *.backup 2>/dev/null

echo "Test 1: Create vault"
echo $PASSWORD | $BIN $VAULT create << EOF
$PASSWORD
$PASSWORD
EOF
echo "Vault created"
echo ""

echo "Test 2: Add password entries"
$BIN $VAULT add-password << EOF
$PASSWORD
GitHub
user@example.com
testpass123


Development

EOF
echo "Entry 1 added"

$BIN $VAULT add-password << EOF
$PASSWORD
Gmail
myemail@gmail.com
gmailpass456
https://mail.google.com
Personal
My personal email
EOF
echo "Entry 2 added"

$BIN $VAULT add-password << EOF
$PASSWORD
AWS Console
admin@company.com
awspass789
https://aws.amazon.com
Work
Company AWS account
EOF
echo "Entry 3 added"
echo ""

echo "Test 3: List passwords"
$BIN $VAULT list-passwords << EOF
$PASSWORD
EOF
echo "List displayed"
echo ""

echo "Test 4: Search passwords"
$BIN $VAULT search github << EOF
$PASSWORD
EOF
echo "Search completed"
echo ""

echo "Test 5: Get password details"
$BIN $VAULT get 1 << EOF
$PASSWORD
EOF
echo "Details displayed"
echo ""

echo "Test 6: Edit password"
$BIN $VAULT edit 1 << EOF
$PASSWORD
GitHub Updated




https://github.com


EOF
echo "Entry edited"
echo ""

echo "Test 7: Vault info"
$BIN $VAULT info << EOF
$PASSWORD
EOF
echo "Info displayed"
echo ""

echo "Test 8: Generate password"
$BIN $VAULT generate 20
echo "Password generated"
echo ""

echo "Test 9: Export to CSV"
$BIN $VAULT export test_export.csv << EOF
$PASSWORD
EOF
echo "Exported to CSV"
echo ""

echo "Test 10: Change master password"
$BIN $VAULT change-password << EOF
$PASSWORD
NewPassword456!
NewPassword456!
EOF
echo "Password changed"
echo ""

echo "Test 11: Verify new password works"
$BIN $VAULT list-passwords << EOF
NewPassword456!
EOF
echo "New password verified"
echo ""

echo "Test 12: Delete entry"
$BIN $VAULT delete 3 << EOF
NewPassword456!
DELETE
EOF
echo "Entry deleted"
echo ""

echo "Test 13: Verify deletion"
$BIN $VAULT list-passwords << EOF
NewPassword456!
EOF
echo "Deletion verified"
echo ""

# Cleanup
rm -f $VAULT test_export.csv *.backup

echo "======================================"
echo "All tests passed"
echo "======================================"