#!/bin/bash

download()(
#make directory for downloaded content
mkdir tmp
cd tmp
rm *.bz2
curl http://konect.uni-koblenz.de/downloads/tsv/ca-cit-HepPh.tar.bz2 -O		
curl http://konect.uni-koblenz.de/downloads/tsv/dblp_coauthor.tar.bz2 -O	
curl http://konect.uni-koblenz.de/downloads/tsv/elec.tar.bz2 -O			
curl http://konect.uni-koblenz.de/downloads/tsv/enron.tar.bz2 -O		
curl http://konect.uni-koblenz.de/downloads/tsv/epinions.tar.bz2 -O		
curl http://konect.uni-koblenz.de/downloads/tsv/facebook-wosn-links.tar.bz2 -O	
curl http://konect.uni-koblenz.de/downloads/tsv/flickr-growth.tar.bz2 -O	
curl http://konect.uni-koblenz.de/downloads/tsv/digg-friends.tar.bz2 -O		
curl http://konect.uni-koblenz.de/downloads/tsv/wikipedia-growth.tar.bz2 -O	
curl http://konect.uni-koblenz.de/downloads/tsv/youtube-u-growth.tar.bz2 -O	
# unzip stuff
for a in `ls -1 *.tar.bz2`; do tar xvjf $a
rm -rf $a ; done
# go to original folder
cd ..
)

parse_input()(	#parse downloaded graphs and put them to data/
for a in `find -maxdepth 3 -name "*out.*"`; do 
echo Parse $a...
java -jar parsegraph.jar $a $input; done
mkdir data
for a in `find -name "*edge_out.*"`; do
mv $a data/$a;done
for a in `find -name "*node_out.*"`; do
mv $a data/$a;done
for a in `find -name "*edge_zerotrans_out.*"`; do
mv $a data/$a;done
)


download_process()(	#download and parse input
download
parse_input
)


init_sources()(
for edge in `ls -r */edge_out*`; do
	./tempath -createList $1 -e $edge;done
for a in `find -name "*source_*"`; do
	mv $a data/;done
)



if [ "$arg" == "download" ]; then
	download_process
elif [ "$arg" == "init_sources" ];then
	if [ "$2" == "" ];then
		echo "Need a second argument. Was empty."
		echo "Example:"
		echo "	./script.sh init_sources 100"
	else	
		init_sources $2
	fi
else
	echo "No Input detected!"
	echo "Use one of the following flags:"
	echo "		- download 	        (download graphs and parse)"
	echo " "
	echo "		- init_sources [x]	(create for each dataset a source file with [x] sources)"
	echo " "
		
fi

#File Documentation:
#	edge_out.xxx		       - File containing edge information
#	n M			               - n: amount of nodes  M: amount of temporal edges (multiple edges between two nodes possible)
#	node node time weight
#	...

#	edge_zerotrans_out.xxx	   - File containing edge information for transmission time 0 
#	n M			                    [Note: Wu's Algorithm only works consistent with Transmission Time]
#	node node time 0

#	node_out.xxx		        - File containing information about the dwell time on nodes
#	n X			                - The value X refers to the mode of creating the data, but has no influence on the program
#	[dwell_time for node 0]	    - X:0 -> All vertices have same dwell_time X:1 -> dwell time is in range of two values
#	...
#  
#The output files of the script are mostly .csv files to use in the Latex file in git. 
#test








