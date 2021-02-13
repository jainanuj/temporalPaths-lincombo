/******************************************************************************
Usage: given an airport dataset, analyses the minimum period by which all the *
temporal edges are repeated                                                   *      
******************************************************************************/
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <string>  // std::string, std::stoi
#include <fstream>
#include <stdio.h> //fopen()
#include <unordered_map>
#include <unordered_set>

using namespace std; 

/*This version of Edge doesn't have "t" info*/
struct Edge{
    int u, v,w; 
    Edge(_u, _v, _w){
	u = _u; 
	v = _v; 
	w = _w; 
    }
};  

/*for dealing with the frequencies*/
unordered_map<Edge, vector<int>> freqMap;

int main(int argc, char* argv[]){
    if(argc < 2){
	cout << "usage: ./airportFreq <file>" << endl;
	return 0; 
    }
    
    FILE* inFile = fopen();
    if(inFile == NULL){
	cout << "error in opening file" << endl;
	return 0;
    }

    int V, numEdges; 
    fscanf(inFile, "%d %d", &V, &numEdges);
    Edge e;
    int t; 
    for(int i = 0; i < numEdges; i++)
    {
        if(fscanf(file, "%d %d %d %d",&e.u, &e.v, &t, &e.w) == EOF){
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
    unordered_map<Edge, vector<int>>::iterator it = freqMap.begin();
    vector<int> vTmp = it->second;
    for(tIndx=1; tIndx < vTmp.size(); tIndex++){
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
        for(tIndx=1; tIndx < vTmp.size(); tIndex++){
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
