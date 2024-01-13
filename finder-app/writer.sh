#!/bin/bash

# Check if the required arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Error: Please provide two arguments - writefile and writestr"
    exit 1
fi

writefile=$1
writestr=$2

# Create the directory if it doesn't exist
mkdir -p "$(dirname "$writefile")"

# Create the file with the specified content
echo "$writestr" > "$writefile"

# Check if the file was created successfully
if [ -e "$writefile" ]; then
    echo "File '$writefile' created/written successfully with content:"
    echo "$writestr"
else
    echo "Error: Could not create/write to file '$writefile'"
    exit 1
fi

