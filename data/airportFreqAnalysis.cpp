/******************************************************************************
Usage: given an airport dataset, analyses the minimum period by which all the *
temporal edges are repeated                                                   *      
******************************************************************************/
#include <cstdio>
#include <algorithm> // std::sort
#include <iostream>
#include <string>  // std::string, std::stoi
#include <fstream>
#include <stdio.h> //fopen()
#include <map>
#include <unordered_set>
#include <vector>

using namespace std; 

/*This version of Edge doesn't have "t" info*/
struct Edge{
    int u, v, w; 
    Edge(){}
    Edge(int _u, int _v, int _w){
	u = _u; 
	v = _v;
	w = _w; 
    }
};  

/*Edges that have all the same parameters except 
for t will end up adjacent in the sorted edge_list*/
struct mycomp {
  bool operator() (Edge e1, Edge e2) {
	if(e1.u != e2.u) return e1.u < e2.u;
	if(e1.v != e2.v) return e1.v < e2.v;
	if(e1.w != e2.w) return e1.w < e2.w;
  }
};


/*for dealing with the frequencies*/
map<Edge, vector<int>, mycomp> freqMap;

int main(int argc, char* argv[]){
    if(argc < 2){
	cout << "usage: ./airportFreq <file>" << endl;
	return 0; 
    }
    
    FILE* inFile = fopen(argv[1], "r");
    if(inFile == NULL){
	cout << "error in opening file" << endl;
	return 0;
    }

    int V, numEdges; 
    int t; 
    fscanf(inFile, "%d %d", &V, &numEdges);
    for(int i = 0; i < numEdges; i++)
    {
    	Edge e;
        if(fscanf(inFile, "%d %d %d %d",&e.u, &e.v, &t, &e.w) == EOF){
	   cout << "error in reading the file" << endl;
	   return 0; 
	}
        freqMap[e].push_back(t); //we assume edges are sorted by t
    }   
    fclose(inFile);

    /*two sets used to take the intersection of all \delta_t values*/
    unordered_set<int> firstSet;
    unordered_set<int> secondSet;

    int tIndx;
    int deltaT;
    map<Edge, vector<int>>::iterator it = freqMap.begin();
    vector<int> vTmp = it->second;
    for(tIndx=1; tIndx < vTmp.size(); tIndx++){
	deltaT = vTmp[tIndx]-vTmp[tIndx-1];
	firstSet.insert(deltaT);
    }
    it++;

    for(; it != freqMap.end(); it++){
	if(firstSet.empty()){
	   cout << "no common period" << endl;
	   return 0;
	}
	vTmp = it->second;
        for(tIndx=1; tIndx < vTmp.size(); tIndx++){
	    deltaT = vTmp[tIndx]-vTmp[tIndx-1];
	    if(firstSet.find(deltaT) != firstSet.end())
	       secondSet.insert(deltaT);
        }
	firstSet.clear();
	swap(firstSet, secondSet);
    }

    if(firstSet.empty()){
	cout << "no common period" << endl;
	return 0; 
    }

    int res = numEdges;
    for(auto it2 = firstSet.begin(); it2 != firstSet.end(); it2++){
	if(*it2 < res)
	   res = *it2;
    }

    cout << "least common period: " << res << endl;

    return 0; 
}
