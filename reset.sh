#!/bin/bash

target=(log votedFor currentTerm )
dir=./src/server/log

nodesize=40

for item in ${target[@]}; do
        echo -n "" > ${dir}/${item}
done


for i in `seq 0 ${nodesize}`;do 
        echo -n "" > ${dir}/log-${i}
done


# rm -r /mnt/ssd01/*
# cp  ${dir}/* /mnt/ssd01/

