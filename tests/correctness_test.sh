#!/bin/bash
SERVER_URL="http://127.0.0.1:8080/"

echo "[*] Running correctness test..."
STATUS=$(curl -s -o /dev/null -w "%{http_code}" $SERVER_URL)

if [ "$STATUS" -eq 200 ]; then
    echo "[PASS] Server responded with HTTP 200"
    exit 0
else 
    echo "[FAIL] Expected HTTP 200, got $STATUS"
    exit 1
fi