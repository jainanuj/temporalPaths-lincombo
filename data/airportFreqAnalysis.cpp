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
    int u, v, t, w; 
    Edge(){}
    Edge(int _u, int _v, int _t, int _w){
	u = _u; 
	v = _v;
	t = _t;
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
	return e1.t < e2.t;
  }
}myCmp;

bool operator == (Edge e1, Edge e2){
	if(e1.u == e2.u && e1.v == e2.v && e1.w == e2.w)
		return true;
	return false;
}

/*for dealing with the frequencies*/
vector<Edge> edge_list;

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
    fscanf(inFile, "%d %d", &V, &numEdges);
    for(int i = 0; i < numEdges; i++)
    {
    	Edge e;
        if(fscanf(inFile, "%d %d %d %d",&e.u, &e.v, &e.t, &e.w) == EOF){
	   cout << "error in reading the file" << endl;
	   return 0; 
	}
        edge_list.push_back(e); //we assume edges are sorted by t
    }   
    fclose(inFile);

    sort(edge_list.begin(), edge_list.end(), myCmp);
      
    vector<vector<int>> freqList;
    vector<Edge>::iterator cur = edge_list.begin();
    vector<Edge>::iterator prev;
    for(; cur != edge_list.end(); cur++){
	if(cur != edge_list.begin() && *cur == *prev)
	   freqList[freqList.size()-1].push_back(cur->t);
        else{
	   vector<int> tmp;
	   tmp.push_back(cur->t);
	   freqList.push_back(tmp);
	} 
	prev = cur;
    }

    /*two sets used to take the intersection of all \delta_t values*/
    unordered_set<int> firstSet;
    unordered_set<int> secondSet;

    int tIndx;
    int deltaT;
    vector<vector<int>>::iterator it = freqList.begin();
    vector<int> vTmp = *it;
    for(tIndx=1; tIndx < vTmp.size(); tIndx++){
	deltaT = vTmp[tIndx]-vTmp[0];
	firstSet.insert(deltaT);
    }
    it++;

    for(; it != freqList.end(); it++){
	if(firstSet.empty()){
	   cout << "no common period" << endl;
	   return 0;
	}
	vTmp = *it;
        for(tIndx=1; tIndx < vTmp.size(); tIndx++){
	    deltaT = vTmp[tIndx]-vTmp[0];
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
