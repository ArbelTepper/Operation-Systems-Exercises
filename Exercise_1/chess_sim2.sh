#!/bin/bash

# This script is used to run the chess simulation program
declare -A game_moves
declare -A board

function copy_board {
 # Copy the board from $1 (source board) to $2 (destination board)

  for ((i=0; i<=8; i++)); do
      for ((j=0; j<=10; j++)); do
        game_moves[$2,$i,$j]=${game_moves[$1,$i,$j]}
      done
  done
  
}

function create_board {
  
  #Initialize the board with dots
  for ((i=1; i<=8; i++)); do
    for ((j=0; j<=8; j++)); do
      game_moves[0,$i,$j]="."
    done
  done

  # Set the row numbers
  for ((i=1; i<=8; i++)); do
    game_moves[0,$i,0]=$[9-i]
    game_moves[0,$i,9]=$[9-i]
  done

  # Set the soldiers
  for ((i=1; i<=8; i++)); do
    game_moves[0,2,$i]="p"
    game_moves[0,7,$i]="P"
  done

  game_moves[0,1,1]="r"; game_moves[0,1,2]="n"; game_moves[0,1,3]="b"; game_moves[0,1,4]="q"; game_moves[0,1,5]="k"
  game_moves[0,1,6]="b"; game_moves[0,1,7]="n"; game_moves[0,1,8]="r"

  game_moves[0,8,1]="R"; game_moves[0,8,2]="N"; game_moves[0,8,3]="B"; game_moves[0,8,4]="Q"; game_moves[0,8,5]="K"
  game_moves[0,8,6]="B"; game_moves[0,8,7]="N"; game_moves[0,8,8]="R"
}

function print_board {
  # Print the board at the "current_move"th element of game_moves
  echo -n "  a b c d e f g h"
  for ((i=0; i<=8; i++)); do
    for ((j=0; j<=10; j++)); do
      echo -n "${game_moves[$1,$i,$j]} " # Print the value of the current board at position i,j
    done
    echo # Print a new line
  done
  echo "  a b c d e f g h"
}

function start_game {
  latest_move=0
  current_move=0

  # create the initial board and place it in the first element of game_moves
  create_board
}

function update_board {

  element=${moves_history[$1]} # Get the ith element of moves_history
  src_col=${element:0:1} # Get the first character
  src_row=${element:1:1} # Get the second character
  dst_col=${element:2:1} # Get the third character
  dst_row=${element:3:1} # Get the fourth character

    #echo
    #echo "Move: $element"

  # Convert the source column to a numeric value
  case $src_col in
    a) src_col=1;;
    b) src_col=2;;
    c) src_col=3;;
    d) src_col=4;;
    e) src_col=5;;
    f) src_col=6;;
    g) src_col=7;;
    h) src_col=8;;
  esac
  # Convert the destination column to a numeric value
  case $dst_col in
    a) dst_col=1;;
    b) dst_col=2;;
    c) dst_col=3;;
    d) dst_col=4;;
    e) dst_col=5;;
    f) dst_col=6;;
    g) dst_col=7;;
    h) dst_col=8;;
  esac
  # Convert the source row i to its inverse (9-i) to match the board
  case $src_row in
    1) src_row=$((9-1));;
    2) src_row=$((9-2));;
    3) src_row=$((9-3));;
    4) src_row=$((9-4));;
    5) src_row=$((9-5));;
    6) src_row=$((9-6));;
    7) src_row=$((9-7));;
    8) src_row=$((9-8));;
  esac
  # Convert the destination row i to its inverse (9-i) to match the board
  case $dst_row in
    1) dst_row=$((9-1));;
    2) dst_row=$((9-2));;
    3) dst_row=$((9-3));;
    4) dst_row=$((9-4));;
    5) dst_row=$((9-5));;
    6) dst_row=$((9-6));;
    7) dst_row=$((9-7));;
    8) dst_row=$((9-8));;
  esac


  # Copy the previous board to the latest board and then make the change on that.
  copy_board $((latest_move - 1)) $latest_move

  # Update the board with the move
    game_moves[$latest_move,$dst_row,$dst_col]=${game_moves[$latest_move,$src_row,$src_col]}
    game_moves[$latest_move,$src_row,$src_col]="."

    # Checking for a castling move

    # Check if the move is a castling move
    if [[ $element == "e1g1" ]]; then
        echo "Castling move: $element move: $latest_move"
        game_moves[$latest_move,$src_row,$((dst_col-1))]="R";
        game_moves[$latest_move,$src_row,$((dst_col+1))]="."
    elif [[ $element == "e1c1" ]]; then
        echo "Castling move: $element move: $latest_move"
        game_moves[$latest_move,$src_row,$((dst_col+1))]="R";
        game_moves[$latest_move,$src_row,$((dst_col-2))]="."
    elif [[ $element == "e8g8" ]]; then
        echo "Castling move: $element move: $latest_move"
        game_moves[$latest_move,$src_row,$((dst_col-1))]="r";
        game_moves[$latest_move,$src_row,$((dst_col+1))]="."
    elif [[ $element == "e8c8" ]]; then
        game_moves[$latest_move,$src_row,$((dst_col+1))]="r";
        game_moves[$latest_move,$src_row,$((dst_col-2))]="."
    fi
    # Checking for a pawn promotion
   if [[ ${#element} -eq 5 ]]; then
        if [[ $dst_row -eq 1 ]]; then
            game_moves[$latest_move,$dst_row,$dst_col]=${promotion^^}
        elif [[ $dst_row -eq 8 ]]; then
            game_moves[$latest_move,$dst_row,$dst_col]=$promotion
        fi
    fi
}

metadata=""
pgn_moves=""
empty_line_reached=false

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

#echo "$uci_moves" > limon_uci.txt

# Split the UCI moves string into an array
read -a moves_history <<< "$uci_moves"
total_moves=${#moves_history[@]}

echo "Metadata from PGN file:"$'\n'"$metadata"

# Run the chess simulation program
start_game

key=""
while [[ $key != "q" ]]; do
    # Print the current move number nad ask the user for input
    echo "Move $current_move/${total_moves}"
    print_board $current_move
    read -p "Press 'd' to move forward, 'a' to move back, 'w' to go to the start, 's' to go to the end, 'q' to quit: " key

    if [[ $key != "a" && $key != "d" && $key != "s" && $key != "w" && $key != "q" ]]; then
        echo "Invalid key pressed: $key"
        continue
    fi
    # Check the key pressed and update the current move accordingly
    if [[ $key == "d" ]]; then
        if [[ $current_move -lt $total_moves ]]; then
            # Check if the latest move is already filled
            if [[ "${game_moves[$((current_move + 1)),1,1]}" == "" ]]; then
              latest_move=$((latest_move+1))
              echo "Latest move: $latest_move"
              # Update the board with the latest move
              update_board $current_move
            fi
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
        for ((; $current_move<=total_moves; current_move++)); do
            if [[ $current_move -lt $total_moves ]]; then
                # Check if the latest move is already filled
                if [[ "${game_moves[$((current_move + 1)),1,1]}" == "" ]]; then
                latest_move=$((latest_move+1))
                # Update the board with the latest move
                update_board $((current_move-1))
                fi
            fi
        done
        current_move=$((current_move-1))   
    fi
done

echo "Exiting."