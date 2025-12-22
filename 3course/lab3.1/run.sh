#!/bin/bash

SRC_DIR="./test_dir"
DST_DIR="./result_dir"

rm -rf "$SRC_DIR" "$DST_DIR"
mkdir -p "$SRC_DIR"


TOP_DIRS=5    
SUB_DIRS=5      
NESTED_DIRS=5   
FILES_PER_DIR=5


for i in $(seq 1 $TOP_DIRS); do
    mkdir -p "$SRC_DIR/dir$i"
    for j in $(seq 1 $SUB_DIRS); do
        mkdir -p "$SRC_DIR/dir$i/subdir$j"
        for k in $(seq 1 $NESTED_DIRS); do
            mkdir -p "$SRC_DIR/dir$i/subdir$j/nest$k"
        done
    done
done


for dir in $(find "$SRC_DIR" -type d); do
    for n in $(seq 1 $FILES_PER_DIR); do
        echo "This is file $n in directory $dir" > "$dir/file$n.txt"
    done
done

echo "дерево  в $SRC_DIR."
