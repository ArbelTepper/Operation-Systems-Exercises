#!/bin/bash

# This script is used to run the chess simulation program

declare -A board

function create_board {
  
  #Initialize the board with dots
  for ((i=1; i<=8; i++)); do
    for ((j=0; j<=8; j++)); do
      board[$i,$j]="."
    done
  done

  # Set the row numbers
  for ((i=1; i<=8; i++)); do
    board[$i,0]=$[9-i]
    board[$i,9]=$[9-i]
  done

  # Set the soldiers
  for ((i=1; i<=8; i++)); do
    board[2,$i]="p"
    board[7,$i]="P"
  done

  board[1,1]="r"; board[1,2]="n"; board[1,3]="b"; board[1,4]="q"; board[1,5]="k"
  board[1,6]="b"; board[1,7]="n"; board[1,8]="r"

  board[8,1]="R"; board[8,2]="N"; board[8,3]="B"; board[8,4]="Q"; board[8,5]="K"
  board[8,6]="B"; board[8,7]="N"; board[8,8]="R"
}

function print_board {
  echo -n "  a b c d e f g h"
  for ((i=0; i<=8; i++)); do
    for ((j=0; j<=10; j++)); do
      echo -n "${board[$i,$j]} " # Print the value of the board at position i,j
    done
    echo # Print a new line
  done
  echo "  a b c d e f g h"
}

# function update_board {}

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







