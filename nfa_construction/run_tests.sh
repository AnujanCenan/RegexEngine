#!/bin/bash

while IFS= read -r line; do
    echo "Processing: $line"
    ./a.out "$line"
    echo
    echo
done < "./test_cases.txt"
