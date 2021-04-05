#!/bin/bash
#for name in airportJan2020.txt arxiv.txt conflict.txt dblp.txt digg.txt elec.txt enron.txt epin.txt fb.txt flicker.txt growth.txt slash.txt youtube.txt 
for name in digg.txt elec.txt enron.txt epin.txt fb.txt flicker.txt growth.txt slash.txt youtube.txt
do
echo "tempath restuls:"
./tempath/tempath fastest "./inputGraphs/${name}" 100 "./initFiles/init_100_${name}"
echo "trans restuls:"
./tempath_trns/tempath fastest "./inputGraphs/${name}" 100 "./initFiles/init_100_${name}"
echo "transB results:"
./tempath_ourTrnsB/tempath fastest "./inputGraphs/${name}" 100 "./initFiles/init_100_${name}"
done
