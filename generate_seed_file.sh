#!/bin/bash
# chmod +x generate_seed_file.sh

echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo ">>> Generating Seed.cpp file"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

SEEDS=()
for i in {1..6}; do
  # Generate a new private key 4096 bits
  openssl genrsa -out seed.tmp 4096
  # Skip the first and last line of the private key file
  TOTAL_LINES=$(wc -l seed.tmp | awk '{print $1}')
  # Store the output of the tail and head commands in the SEED variable
  SEED=$(tail -n +2 seed.tmp | head -n $((TOTAL_LINES - 2)))
  SEEDS+=( "$SEED" )
  # Clean up
  rm seed.tmp
done

# Create the timestamp
TIMESTAMP=$(date +"%Y%m%d%H%M%S")

# Create the output file
FILE_NAME="turtlpass-firmware/Seed.cpp.${TIMESTAMP}"
touch "$FILE_NAME"

# Write the contents of the file
cat > "$FILE_NAME" << EOF
#include "Seed.h"
EOF

for i in {0..5}; do
  cat >> "$FILE_NAME" << EOF
const char* seed$i PROGMEM = R"key(
${SEEDS[$i]}
)key";
EOF
done

cat >> "$FILE_NAME" << EOF
const char* seedArray[] PROGMEM = {seed0, seed1, seed2, seed3, seed4, seed5};
EOF

echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo ">>> Generated file: $FILE_NAME"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Done"
