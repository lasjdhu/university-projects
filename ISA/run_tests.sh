#!/bin/sh

set -e

DNS="./dns"
FILTER="./examples/1.txt"
SERVER_IP="127.0.0.1"
PORT=53535

PASSED=0
FAILED=0

run_query() {
    dig @$SERVER_IP -p $PORT $1 A +short
}

if $DNS -f "$FILTER" >/dev/null 2>&1; then
    echo "FAILED: missing -s did not fail"
    FAILED=$((FAILED+1))
else
    echo "PASSED: missing -s failed as expected"
    PASSED=$((PASSED+1))
fi

if $DNS -s 8.8.8.8 >/dev/null 2>&1; then
    echo "FAILED: missing -f did not fail"
    FAILED=$((FAILED+1))
else
    echo "PASSED: missing -f failed as expected"
    PASSED=$((PASSED+1))
fi

$DNS -s 8.8.8.8 -p $PORT -f $FILTER &
DNS_PID=$!
trap 'kill $DNS_PID 2>/dev/null || true; exit' EXIT

OUTPUT=$(run_query example.com)
if [ -z "$OUTPUT" ]; then
    echo "FAILED: forwarded query returned empty"
    FAILED=$((FAILED+1))
else
    echo "PASSED: forwarded query succeeded"
    PASSED=$((PASSED+1))
fi

BLOCKED=$(head -n1 "$FILTER")
OUTPUT=$(run_query "$BLOCKED")
if [ -z "$OUTPUT" ]; then
    echo "PASSED: blocked query returned no A record"
    PASSED=$((PASSED+1))
else
    echo "FAILED: blocked query returned $OUTPUT"
    FAILED=$((FAILED+1))
fi

OUTPUT=$(dig @$SERVER_IP -p $PORT example.com AAAA +short)
if [ -z "$OUTPUT" ]; then
    echo "PASSED: non-A query rejected"
    PASSED=$((PASSED+1))
else
    echo "FAILED: non-A query returned $OUTPUT"
    FAILED=$((FAILED+1))
fi

kill $DNS_PID
wait $DNS_PID 2>/dev/null || true

echo ""
echo "Tests finished: $PASSED passed, $FAILED failed"

if [ $FAILED -ne 0 ]; then
    exit 1
else
    exit 0
fi

