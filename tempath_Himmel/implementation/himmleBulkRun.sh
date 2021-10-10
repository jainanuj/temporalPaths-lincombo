#!/bin/bash
for name in airportJan2020.txt arxiv.txt conflict.txt digg.txt elec.txt enron.txt epin.txt fb.txt flicker.txt growth.txt slash.txt youtube.txt 
do
echo "dataset: ${name}"
echo "Himmel restuls:"
./tempath -e "../../inputGraphs/edge_zerotrans_${name}" -f "../../initFiles/init_100_${name}" -a foremost
done

