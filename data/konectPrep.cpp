#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream> 
#include <fstream>
#include <time.h> /* time_t, struct tm, time, gmtime */
#include <cstddef>  /* std::size_t */
#include <unordered_map>
#include <algorithm> /* std::sort */
using namespace std; 

struct Edge{
  int u, v, w; 
  long t; 
  Edge(int _u, int _v, long _t, int _w){
	u = _u; 
	v = _v; 
	t = _t; 
	w = _w; 
  }
};

struct edgeSort {
  bool operator() (Edge e1, Edge e2) { return (e1.t < e2.t);}
} eSort;

vector<Edge> edgeList;

int main(int argc, char *argv[]) 
{  
    bool undirected = false;
    if(argc < 3){
	cout << "useage: ./airportPrep <path to the input file> <path to the output file>, -u [optinal - if undirected]" << endl; 
	return 0; 
    }   

    if(argc == 4)
	undirected = true;

    string line, word; 

    // Open the input file 
    fstream fin;
    fin.open(argv[1], ios::in); 
    if(!fin.is_open()){
	cout << "Error opening the file";
	return 0; 
    }	  

    // Read the Data from the file 
    // as String Vector 
    vector<string> row;
 
    while (getline(fin, line)) {   
        row.clear(); 
	if(line.size() == 0)
	   break;
  
        // used for breaking words 
        stringstream ss(line); 
  
        while (getline(ss, word, ' ')) { 
            row.push_back(word); 
        } 

	/*bypassing the titles: lines that start with %*/
        if(row[0].compare("%") != 0){
	   Edge e(stoi(row[0]), stoi(row[1]), stol(row[3]), 1);

	   //bypassing the self-cycles:
	   if(e.u == e.v)
	      continue;

	   edgeList.push_back(e);
	   if(undirected){
	      Edge e2(stoi(row[1]), stoi(row[0]), stol(row[3]), 1);
	      edgeList.push_back(e2);
	   }
  	}
	line = "";
    } 

    cout << "here" << endl; 

    fin.close();
    cout << "loc-1" << endl;
    /*sort the edgeList in increasing order of t*/
    sort(edgeList.begin(), edgeList.end(), eSort);


    for(long i=0; i<edgeList.size(); i++)
	cout << edgeList[i].u << " " << edgeList[i].v << " " << edgeList[i].t << " " << edgeList[i].w << endl;

    /*modify the time stamps. subtract all by the min time. If
    the min time is 0, replace the min time by the second min
    time - 10*/
    long index = 0; 
    while(edgeList[index].t == 0) //Assuem there is at least 1 non-zero timestamp
	index++;
    cout << "loc0" << endl;
    if(index > 0){
	for(long i=index-1; i>=0; i--)
	    edgeList[i].t = edgeList[index].t-10;
    }

    cout << "loc1" << endl;
    
    /*Here, min time is subtracted from all the times
     Also, the nodes are both counted and re-numbered*/
    unordered_map<int, int> node_map;
    unordered_map<int, int>::iterator it;
    int node_index = 0; 

    long minT = edgeList[0].t; //min time
    for(long i=0; i<edgeList.size(); i++){ 
        //first modify the time
	edgeList[i].t -= minT;
	 
	//now, modify the node indices
	//first deal with u
	it = node_map.find(edgeList[i].u);
	if(it == node_map.end()){
	   node_map[edgeList[i].u] = node_index;
	   edgeList[i].u = node_index++;
	}else{
	   edgeList[i].u = node_map[edgeList[i].u];
	}

	//now deal with v
	it = node_map.find(edgeList[i].v);
	if(it == node_map.end()){
	   node_map[edgeList[i].v] = node_index;
	   edgeList[i].v = node_index++;
	}else{
	   edgeList[i].v = node_map[edgeList[i].v];
	}
	
    }

    /*now write the edgelist to an output file*/
    ofstream outfile;
    outfile.open(argv[2]);
    if(!outfile.is_open())
	cout << "error in opening the outfile" << endl;

    outfile << node_index << " " << edgeList.size() << endl;
    for(int i=0; i<edgeList.size(); i++)
	outfile << edgeList[i].u << " " << edgeList[i].v << " " << edgeList[i].t << " " << edgeList[i].w << endl; 

    outfile.close();
    return 0;  
}   
