#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "graph.h"
using namespace std;

int main(int argc, char* argv[]){

    /*added by sanaz*/
    if(argc < 4){
	cout << "usage: ./tempath <algorithm> <path to the edge_list> <number of source nodes> <path to the init file>[optional]" << endl; 
	return 0; 
    }
	
    char* option = argv[1];	
    Graph g(argv[2]);

    /*modified by sanaz*/
    int numS = stoi(argv[3]);
    if(numS <= 0){
	cout << "number of source nodes should be a positive integer" << endl;
	return 0; 
    }
    if(argc == 5)
	g.initial_query(argv[4], numS);
    else
	g.initial_query(numS);


    if(!strcmp(option,"earliest"))
    {
		g.run_earliest_arrival();
    }
    else if (!strcmp(option,"latest"))
    {
		g.run_latest_departure();
    }
    else if (!strcmp(option,"shortest"))
    {
		g.run_shortest();
    }
    else if(!strcmp(option,"fastest"))
    {
		g.run_fastest();
    }
	
    //cout<<"hello world!!!" <<endl; //removed by sanaz, I see no reason for this statement
	
    return 0;
}



