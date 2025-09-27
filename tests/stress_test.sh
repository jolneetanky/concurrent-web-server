#!/bin/bash
SERVER_URL="http://127.0.0.1:8080/"

run_benchmark() {
    # Parse CLI arguments
    NAME=$1
    THREADS=$2
    CONNS=$3
    DURATION=$4

    echo "[*] Running $NAME load: ${THREADS} threads, ${CONNS} connections, ${DURATION}..."

    OUTPUT=$(wrk -t$THREADS -c$CONNS -d$DURATION $SERVER_URL 2>&1) # $(command) Runs a command in a subshell and channels the output in to here

    # `Latency line looks like this:`
    # Avg, Stdev, Highest, +/- stddev
    # Latency   18.10ms    2.91ms   58.12ms   85.99%

    # Extract metrics
    AVG_LAT=$(echo "$OUTPUT" | grep "Latency" | awk '{print $2}') # Extract the line containing "Latency", and extract stats from there
    # `awk` splits the line into fields, each of which we can access (1-indexed), eg. $1, $2, etc.
    MAX_LAT=$(echo "$OUTPUT" | grep "Latency" | awk '{print $4}')
    # REQ_SEQ=$(echo "$OUTPUT" | grep "Requests/sec" | awk '{print $3}')
    REQ_SEC=$(echo "$OUTPUT" | awk '/Requests\/sec/ {print $NF}')
    TRANSFER=$(echo "$OUTPUT" | grep "Transfer/sec" | awk '{print $2, $3}')
    ERRORS=$(echo "$OUTPUT" | grep "Socket errors" | sed 's/.*errors: //')

    echo "=== $NAME Results ==="
    echo "Average Latency : $AVG_LAT"
    echo "Max Latency     : $MAX_LAT"
    echo "Requests/sec    : $REQ_SEC"
    echo "Transfer/sec    : $TRANSFER"
    echo "Socket Errors   : $ERRORS"
    echo "======================"
    echo

    # echo "$OUTPUT" | grep "Requests/sec"

}

# Low load
run_benchmark "LOW" 1 5 20s

# Medium load
run_benchmark "MEDIUM" 4 500 20s

# High load
run_benchmark "HIGH" 5 1000 20s

# Highest load
run_benchmark "HIGHEST" 5 2000 20s