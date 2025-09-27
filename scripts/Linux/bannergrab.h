#pragma once

#import <string>

// For Linux

const std::string bannerScript{R"(#!/bin/bash
#
# banner_grab.sh – Banner grab with CSV output, optimized stealth scan
# Usage:
#   ./banner_grab.sh [fast|full|stealth] targets.txt [nmap_path]
# Example:
#   ./banner_grab.sh full targets.txt /usr/local/bin/nmap

set -e

MODE=$1
TARGET_FILE=$2
NMAP_PATH=${3:-nmap} # <--- NEW: Nmap path variable, defaults to 'nmap' if not provided

# Define port sets
FAST_PORTS="22,80,443,135,445,3389"
FULL_PORTS="22,80,443,135,139,445,3389,5985,5986,25,53,389,636,1433,3306,5432,8080,8443,515,9100,161,162"
STEALTH_PORTS="22,80,443,135,139,445,3389,5985,5986,25,53,389,636,1433,3306,5432,8080,8443"

# Timestamp for CSV
TIMESTAMP=$(date +'%Y-%m-%d_%H-%M-%S')

# Choose ports, speed, and output file
case "$MODE" in
  fast)
    PORTS=$FAST_PORTS
    OUTPUT="banner_results_fast_${TIMESTAMP}.csv"
    SPEED="-T4"
    ;;
  full)
    PORTS=$FULL_PORTS
    OUTPUT="banner_results_full_${TIMESTAMP}.csv"
    SPEED="-T3"
    ;;
  stealth)
    PORTS=$STEALTH_PORTS
    OUTPUT="banner_results_stealth_${TIMESTAMP}.csv"
    SPEED="-sS -T2 --max-retries 1 --host-timeout 30s"
    ;;
  *)
    echo "Invalid mode. Use fast|full|stealth."
    echo "Usage: $0 [fast|full|stealth] targets.txt [nmap_path]" # <--- NEW: Updated usage message
    exit 1
    ;;
esac

echo "[*] Running $MODE scan on targets in $TARGET_FILE"
echo "================================================="

# CSV header
echo '"IP","Hostname","OS","Role","ScanType","Port","State","Service","Banner"' > "$OUTPUT"

# Map ports to roles
get_role() {
  case "$1" in
    22) echo "SSH/Admin" ;;
    80|443|8080|8443) echo "Web" ;;
    3306|5432|1433) echo "Database" ;;
    25|53|389|636|5985|5986) echo "Service" ;;
    135|139|445|515|9100|161|162|3389) echo "Network/Print" ;;
    *) echo "Other" ;;
  esac
}

# Loop through targets
while IFS= read -r TARGET; do
  [ -z "$TARGET" ] && continue

  echo "Attempting to run Nmap..."
  echo "Nmap path is: $NMAP_PATH"
  echo "Nmap command is: sudo $NMAP_PATH $SPEED -Pn --reason -p\"$PORTS\" \"$TARGET\" -oG -"

  echo
  echo "========== TARGET: $TARGET =========="
  echo "--- $MODE SCAN ---"

  # Resolve hostname
  HOSTNAME=$(nslookup "$TARGET" 2>/dev/null | awk -F'= ' '/name/ {gsub(/\.$/,"",$2); print $2}')
  [ -z "$HOSTNAME" ] && HOSTNAME="$TARGET"

  # Detect OS using Nmap
  OS=$($NMAP_PATH -O -Pn "$TARGET" 2>/dev/null | grep "OS details" | awk -F': ' '{print $2}') # <--- MODIFIED: Use NMAP_PATH variable
  [ -z "$OS" ] && OS="Unknown"



  # Run Nmap scan
  # Run Nmap scan and store the output in a variable
NMAP_OUTPUT=$(sudo "$NMAP_PATH" $SPEED -Pn --reason -p"$PORTS" "$TARGET" -oG -)

# Now, parse the output line by line
while read -r line; do
  if [[ $line == *Ports* ]]; then
    IP=$(echo "$line" | awk '{print $2}')
    PORTS_INFO=$(echo "$line" | grep -oP '\d+/(open|closed|filtered)/\S+')

    for PORTINFO in $PORTS_INFO; do
      PORT=$(echo "$PORTINFO" | cut -d/ -f1)
      STATE=$(echo "$PORTINFO" | cut -d/ -f2)
      SERVICE=$(echo "$PORTINFO" | cut -d/ -f3)

      # Grab banner only if open
      if [ "$STATE" == "open" ]; then
        BANNER=$(echo | nc -w1 "$TARGET" "$PORT" 2>/dev/null | tr '\r\n' ' ')
      else
        BANNER="N/A"
      fi

      ROLE=$(get_role "$PORT")

      # Print console output
      echo "[$IP] Port $PORT ($STATE) - Service: $SERVICE - Banner: $BANNER - Role: $ROLE"

      # Append to CSV
      echo "\"$IP\",\"$HOSTNAME\",\"$OS\",\"$ROLE\",\"$MODE\",\"$PORT\",\"$STATE\",\"$SERVICE\",\"$BANNER\"" >> "$OUTPUT"
    done
  fi
done <<< "$NMAP_OUTPUT"

  echo "========== END TARGET: $TARGET =========="

done < "$TARGET_FILE"

echo "[*] $MODE scan completed. Results saved to $OUTPUT")" };
