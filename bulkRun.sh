#!/bin/bash
for name in airportJan2020.txt arxiv.txt conflict.txt digg.txt elec.txt enron.txt epin.txt fb.txt flicker.txt growth.txt slash.txt youtube.txt 
#for name in slash.txt youtube.txt
do
echo "dataset: ${name}"
#echo "tempath restuls:"
#./tempath/tempath earliest "./inputGraphs/${name}" 100 "./initFiles/init_100_${name}"
echo "trans restuls:"
./tempath_trns/tempath earliest "./inputGraphs/${name}" 100 "./initFiles/init_100_${name}"
echo "transB results:"
./tempath_ourTrnsB/tempath earliest "./inputGraphs/${name}" 100 "./initFiles/init_100_${name}"

#For transA, the optimization criteria and the initFile do not matter
echo "transA results (|V| and |E| in TRG_Zchoche):"
./tempath_ourTrnsA/tempath earliest "./inputGraphs/${name}" 100 "./initFiles/init_100_${name}"
done
