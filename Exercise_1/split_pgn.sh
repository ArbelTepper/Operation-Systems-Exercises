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
    counter=$1
    SPLIT=$0
    current_game=""

    while IFS= read -r line; do
    # Define the output file name
    output_file="$SOURCE_$counter.pgn"
    
    stats = read -r -d '' data
    echo "$stats"


    # Write the line to the output file
    
    #echo "$line" > "$output_file"
    
    # Increment the counter
    counter=$((counter + 1))
done < "$input_file"

}


args_num="$#"
args="$@"
echo "the args are $args"


varify_num_of_args $args_num
varify_source_file_exist $args
check_directory_exist $args

#split_pgn