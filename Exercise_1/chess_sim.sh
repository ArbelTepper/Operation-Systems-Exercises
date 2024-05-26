#!/bin/bash

# This script is used to run the chess simulation program

metadata=""
pgn_moves=""
empty_line_reached=false
current_move=0


# Read the file line by line and separate the metadata and the moves when the first empty line is found.
while IFS= read -r line || [[ -n $line ]]
do
  if [[ -z $line ]]; then
    empty_line_reached=true
    continue
  fi

  if $empty_line_reached; then
    pgn_moves+="$line"$'\n'
  else
    metadata+="$line"$'\n'
  fi
done < "$1"
echo "$pgn_moves" > "pgn_moves.txt"

# Parse the moves from the PGN file to UCI format
uci_moves=$(python3 "parse_moves.py" "$pgn_moves")

# Split the UCI moves string into an array
read -a moves_history <<< "$uci_moves"
total_moves=${#moves_history[@]}

echo "Metadata from PGN file:"$'\n'"$metadata"

# Run the chess simulation program
key=""
while [[ $key != "q" ]]; do
    # Print the current move number nad ask the user for input
    echo "Move $current_move/${total_moves}"
    read -p "Press 'd' to move forward, 'a' to move back, 'w' to go to the start, 's' to go to the end, 'q' to quit: " key

    if [[ $key != "a" && $key != "d" && $key != "s" && $key != "w" && $key != "q" ]]; then
        echo "Invalid key pressed: $key"
        continue
    fi
    # Check the key pressed and update the current move accordingly
    if [[ $key == "d" ]]; then
        if [[ $current_move -lt $total_moves ]]; then
            current_move=$((current_move+1))
        else
            echo "No more moves available."
        fi
    elif [[ $key == "a" ]]; then
        if [[ $current_move -gt 0 ]]; then
            current_move=$((current_move-1))
        else
            # If no moves have been made yet (i.e., the board is in the initial state),
            # pressing 'a' will have no effect, and no changes will occur on the board.
            continue
        fi
    elif [[ $key == "w" ]]; then
        current_move=0
    elif [[ $key == "s" ]]; then
        current_move=${#moves_history[@]}
    fi
    # print the board
done

echo "Exiting."







