#!/usr/bin/env bash

echo "Choose an option:"
echo "Option 1: no arguments"
echo "Option 2: one argument"
echo "Option 3: two arguments - file does not exist"
echo "Option 4: two arguments - no problems"

read choice

if [ $choice -eq 1 ]; then
    ./split_pgn.sh
elif [ $choice -eq 2 ]; then
    ./split_pgn.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24.pgn
elif [ $choice -eq 3 ]; then
    ./split_pgn.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24-1.pgn new_folder
elif [ $choice -eq 4 ]; then
    ./split_pgn.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24.pgn new_folder
else
    echo "Invalid choice."
fi
