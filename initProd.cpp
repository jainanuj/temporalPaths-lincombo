/*****************************************************************************
Usage: given the number of nodes |V| and assuming the nodes are numbered     *
from 0 to |V|-1, produces "init_points" source nodes from which to compute   *
the single pair shortest paths. "init_file" is the file to which the initial *
points are written.                                                          *
*****************************************************************************/
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <string>  // std::string, std::stoi
#include <fstream>

using namespace std; 

int main(int argc, char* argv[]){
    if(argc < 4){
	cout << "usage: ./initProd <init_points> <|V|> <init_file>" << endl;
	return 0; 
    }

    int cnt = stoi(argv[1]);	
    int V = stoi(argv[2]);
    int s;

    ofstream initFile(argv[3]);
    if(!initFile.is_open()){
	cout << "error in openning file" << endl;
	return 0; 
    }

    for(int i = 0; i < cnt ;i ++){
    	s=rand()%V;
        initFile << s << endl;
    }

    initFile.close();
    return 0; 
}
