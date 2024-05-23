#!/usr/bin/env bash

echo "Choose an option:"
echo "Option 1: no arguments"
echo "Option 2: one argument"
echo "Option 3: two arguments - file does not exist"
echo "Option 4: two arguments - no problems"
echo "Option 5: test of grep and machamism with a new folder"
echo "Option 6: test grep"
echo "Option 7: test Adam's split"

read choice

if [ $choice -eq 1 ]; then
    ./split_pgn.sh
elif [ $choice -eq 2 ]; then
    ./split_pgn.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24.pgn
elif [ $choice -eq 3 ]; then
    ./split_pgn.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24-1.pgn test_folder
elif [ $choice -eq 4 ]; then
    ./split_pgn.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24.pgn test_folder
elif [ $choice -eq 5 ]; then
    ./split_pgn.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24.pgn test_folder
elif [ $choice -eq 6 ]; then
    grep -Pzo '\[Event\b\s+(?:(?!\[Event\b)[\s\S])*' "/home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24.pgn" 
elif [ $choice -eq 7 ]; then
    ./Adam_split.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24.pgn test_folder


    #grep -Pzo '(\[\w+ ".*?"]\n)+\n(\d+[^[]+)' "/home/arbel/Operation-Systems-Exercises/Exercise_1/pgns/capmemel24.pgn"
    # | awk '{ORS = NR%2 ? "\n" : "\n--- Match Separator ---\n"} 1'
else
    echo "Invalid choice."
fi


