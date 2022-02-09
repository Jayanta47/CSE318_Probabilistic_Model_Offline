#!/bin/bash
i=0
for filename in ./cases/*; do
    echo $filename;
    if [[ -d $i ]] 
    then 
        rm -rf $i;
    fi
    mkdir $i;
    ./Main.o $filename ./$i/output.txt
    mv ./*.txt ./$i/
    i=$((i+1));
done;