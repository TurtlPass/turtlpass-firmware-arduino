#!/bin/bash
# chmod +x generate-seed.sh

echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo ">>> Generating Seed.cpp file"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"

# Define the output file name
FILE_NAME="../turtlpass-firmware/Seed.cpp"

# Check if the file already exists
if [ -e "$FILE_NAME" ]; then
  echo "Error: $FILE_NAME already exists. Aborting."
  exit 1
fi

SEEDS=()
for i in {1..9}; do
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

# Create the output file
touch "$FILE_NAME"

# Write the contents of the file
cat > "$FILE_NAME" << EOF
#include "Seed.h"
EOF

for i in {0..8}; do
  cat >> "$FILE_NAME" << EOF
const char* seed$i PROGMEM = R"key(
${SEEDS[$i]}
)key";
EOF
done

# Write the seedArray using a loop
cat >> "$FILE_NAME" << EOF
const char* seedArray[] PROGMEM = {
EOF

for i in {0..8}; do
  echo "seed$i," >> "$FILE_NAME"
done

cat >> "$FILE_NAME" << EOF
};
EOF

echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo ">>> Generated file: $FILE_NAME"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Done"
