#!/bin/bash

# Script to Split PGN Files into Individual Game Files
# It verifies the number of arguments, checks if the source file exists, ensures the destination directory 
# exists (creating it if necessary), and performs the splitting operation.


# Function to verify the number of arguments provided to the script
# Input: $1 - Number of arguments passed to the script
# Output: None
# Exit Code: 1 if the number of arguments is not equal to 2

function varify_num_of_args() {

    num=$1

    if [ $num -ne 2 ]; then
        echo "Usage: $0 <source_pgn_file> <destination_directory>"
        exit 1
    fi

}

# Function to verify that the source file exists
# Input: $1 - The full path of the source file
# Output: None
# Exit Code: 1 if the file does not exist

function varify_source_file_exist() {
    # SOURCE is given the full path of a file.

    SOURCE=$1
    
    if [ ! -f "$SOURCE" ]; then
        echo "Error: File '$SOURCE' does not exist."
        exit 1
    fi

}


# Function to check if the destination directory exists, and create it if it does not
# Input: $2 - The path of the destination directory
# Output: Creates the directory if it does not exist and prints a message
# Exit Code: None

function check_directory_exist() {

    DIRECTORY=$2

    if [ ! -d "$DIRECTORY" ]; then
        echo "Created directory '$DIRECTORY'."
        mkdir -p "$DIRECTORY"
    fi

}
# Function to split the PGN file into individual game files
# Input: $1 - The full path of the source PGN file
#        $2 - The path of the destination directory
# Output: Creates individual game files in the destination directory and prints messages for each file saved
function split_pgn() {

    SOURCE=$1
    DIRECTORY=$2
    COUNTER=1

    file_name=$(basename -- "$SOURCE")
    file_name="${file_name%.*}"

    # Use grep with regular expressions to match each game in the PGN file
    mapfile -d '' matches < <(grep -Pzo '\[Event\b\s+(?:(?!\[Event\b)[\s\S])*' "$SOURCE")
    
    for match in "${matches[@]}"; do
        
        echo -e "$match\n" > $DIRECTORY/temp.pgn
        head -n -2 "${DIRECTORY}/temp.pgn" > "temp.pgn" && mv "temp.pgn" "${DIRECTORY}/${file_name}_${COUNTER}.pgn"
        echo "Saved game to ${DIRECTORY}/${file_name}_${COUNTER}.pgn"
        # Increment the counter
        COUNTER=$((COUNTER + 1))
    done
    rm $DIRECTORY/temp.pgn
}


# Main Script

args_num="$#"
args="$@"

varify_num_of_args $args_num
varify_source_file_exist $args
check_directory_exist $args
split_pgn $args