#!/usr/bin/env bash

varify_num_of_args() {

    num=$1

    if [ $num -ne 2 ]; then
        echo "Usage: $0 <source_pgn_file> <destination_directory>"
        exit 1
    fi

}

varify_source_file_exist() {
    # SOURCE is given the full path of a file.

    SOURCE=$1
    
    if [ ! -f "$SOURCE" ]; then
        echo "Error: File '$SOURCE' does not exist."
        exit 1
    fi

}

check_directory_exist() {

    DIRECTORY=$2

    if [ ! -d "$DIRECTORY" ]; then
        echo "Created directory '$DIRECTORY'."
        mkdir -p "$DIRECTORY"
    fi

}

split_pgn() {

    SOURCE=$1
    DIRECTORY=$2
    COUNTER=1

    matches=()

    while IFS= read -r line; do
        matches+=("$line")
    done < <(grep "Event" $SOURCE) # Change regex to match the GPN format of a single game.

    # Print all matches
    for match in "${matches[@]}"; do
        echo $match > $DIRECTORY/$SOURCE_$COUNTER.pgn
        echo "Saved game to $DIRECTORY/$SOURCE_$COUNTER.pgn"
        # Increment the counter
        COUNTER=$((COUNTER + 1))
    done
}

args_num="$#"
args="$@"

varify_num_of_args $args_num
varify_source_file_exist $args
check_directory_exist $args

split_pgn $args