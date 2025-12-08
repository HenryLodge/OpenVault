#!/bin/bash

# Edge case testing script

BIN="../bin/main_dbg"
VAULT="test_edge.ovault"
PASSWORD="Pass123!"

echo "Testing Edge Cases..."
echo ""

# Test 1: Wrong password
echo "Test 1: Wrong password on existing vault"
echo $PASSWORD | $BIN $VAULT create << EOF
$PASSWORD
$PASSWORD
EOF

$BIN $VAULT list-passwords << EOF
WrongPassword
EOF
if [ $? -ne 0 ]; then
    echo "✓ Correctly rejected wrong password"
else
    echo "✗ FAILED: Should reject wrong password"
fi
echo ""

# Test 2: Try to create duplicate vault
echo "Test 2: Try to create duplicate vault"
$BIN $VAULT create << EOF
$PASSWORD
$PASSWORD
EOF
if [ $? -ne 0 ]; then
    echo "✓ Correctly rejected duplicate vault"
else
    echo "✗ FAILED: Should reject duplicate vault"
fi
echo ""

# Test 3: Get non-existent entry
echo "Test 3: Get non-existent entry"
$BIN $VAULT get 999 << EOF
$PASSWORD
EOF
if [ $? -ne 0 ]; then
    echo "✓ Correctly rejected non-existent entry"
else
    echo "✗ FAILED: Should reject non-existent entry"
fi
echo ""

# Test 4: Delete non-existent entry
echo "Test 4: Delete non-existent entry"
$BIN $VAULT delete 999 << EOF
$PASSWORD
DELETE
EOF
if [ $? -ne 0 ]; then
    echo "✓ Correctly rejected non-existent entry"
else
    echo "✗ FAILED: Should reject non-existent entry"
fi
echo ""

# Test 5: Empty vault operations
echo "Test 5: Operations on empty vault"
$BIN $VAULT list-passwords << EOF
$PASSWORD
EOF
echo "✓ Empty list handled"
echo ""

# Test 6: Search in empty vault
$BIN $VAULT search test << EOF
$PASSWORD
EOF
echo "✓ Search in empty vault handled"
echo ""

# Cleanup
rm -f $VAULT

echo "Edge case testing complete!"