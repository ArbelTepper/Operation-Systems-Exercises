#!/bin/bash

#./chess_sim.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/Part_1/splited_pgn/capmemel24_6.pgn

#test 6
input="/home/arbel/Operation-Systems-Exercises/Exercise_1/tester/filtered_games/Alburt/Alburt_563.pgn"

moves="ddamawshdzfawiwadawdsdwsddwaadwwscladaq"

#echo "$moves" | ./chess_sim.sh "$input" > test_1_output.txt 2>&1

echo "$moves" | ./chess_sim.sh "$input" > new_test_6.txt 2>&1