#!/bin/bash

#./chess_sim.sh /home/arbel/Operation-Systems-Exercises/Exercise_1/Part_1/splited_pgn/capmemel24_1.pgn


input="/home/arbel/Operation-Systems-Exercises/Exercise_1/Part_1/splited_pgn/capmemel24_7.pgn"
moves="dwwvcvdvwccddwxxdcxvvxxwxdwdsvdswvvsssxzxxvzxcwcdq"

#echo "$moves" | ./chess_sim.sh "$input" > test_1_output.txt 2>&1

echo "$moves" | ./chess_sim.sh "$input" > test_7_output.txt 2>&1