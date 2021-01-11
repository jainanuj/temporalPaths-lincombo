tempath: tempath.o graph.o
	g++ -O3  tempath.o graph.o -o tempath

graph.o: graph.cpp 
	g++ -std=c++11 -Wno-deprecated -O3 -c  graph.cpp

tempath.o: tempath.cpp
	g++ -std=c++11 -Wno-deprecated -O3 -c  tempath.cpp

clean: 
	rm *.o
