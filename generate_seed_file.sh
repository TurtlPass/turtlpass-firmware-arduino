#!/bin/bash
# chmod +x generate_seed_file.sh

echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo ">>> Generating Seed.cpp file"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

# Generate a new private key
openssl genrsa -out seed.tmp 4096

# Skip the first and last line of the private key file
TOTAL_LINES=$(wc -l seed.tmp | awk '{print $1}')

# Store the output of the tail and head commands in the SEED variable
SEED=$(tail -n +2 seed.tmp | head -n $((TOTAL_LINES - 2)))

# Create the timestamp
TIMESTAMP=$(date +"%Y%m%d%H%M%S")

# Create the output file
FILE_NAME="turtlpass-firmware/Seed.cpp.${TIMESTAMP}"
touch "$FILE_NAME"

# Write the contents of the file
cat > "$FILE_NAME" << EOF
#include "Seed.h"

const char* seed PROGMEM = R"key(
$SEED
)key";
EOF

# Clean up
rm seed.tmp

echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "$SEED"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo ">>> Generated file: $FILE_NAME"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Done"
