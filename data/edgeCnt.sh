#!/bin/bash

bcfile="sumTmp.txt"
echo -n "" > "$bcfile"

#for file in $(ls ./files/)
#do
#    bzip2 -d -k ./files/"${file}"
#done

echo -e "sum = 0\n" >> "$bcfile"
for file in $(ls ./files/*.permuted)
do
    echo -e "sum = sum + $(cat $file | wc -l)\n" >> "$bcfile"
done
echo -e "print sum\n" >> "$bcfile"
  
s=`cat "$bcfile" | bc -l`
echo total number of lines: $s  
