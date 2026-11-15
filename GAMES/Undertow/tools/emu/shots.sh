#!/bin/sh
# shots.sh <hex> <bin> <port> <prefix> : boot, new game, opening dialogue, into the map
./drive "$1" "$2" "$3" "w2500 s${4}_1title A w1500 s${4}_2card A w800 s${4}_3talk A w300 A w300 A w300 A w300 A w300 A w300 A w300 A w300 A w2500 s${4}_4map D w300 s${4}_5cursor"
