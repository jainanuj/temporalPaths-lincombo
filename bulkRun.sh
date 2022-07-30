#!/bin/bash
#for name in airportJan2020 arxiv conflict digg elec enron epin fb flicker growth slash youtube 
#for name in slash youtuba
for name in test
do
echo "dataset: ${name}"
echo "linear results:"
./tempath_ourTrnsB/tempath linear "./inputGraphs/${name}.txt" 3 "./initFiles/init_3_${name}.txt"
#./tempath_ourTrnsB/tempath linear "./inputGraphs/${name}.txt" 100 "./initFiles/init_100_${name}.txt"
mv "./results_"* "./${name}_res/"
mv "./timings" "./${name}_res/"
done
