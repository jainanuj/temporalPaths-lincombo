#include <iostream>
#include <vector>
#include <string>
#include <sstream> 
#include <fstream>
using namespace std; 

struct Edge{
  int u, v, t, w; 
  Edge(int _u, int _v, int _t, int _w){
	u = _u; 
	v = _v; 
	t = _t; 
	w = _w; 
  }
};

void process(vector<string> row);

int main() 
{   
    // File pointer 
    fstream fin; 
  
    // Open an existing file 
    fin.open("./AirportRawData/jan2020.csv", ios::in); 
    if(!fin.is_open()){
	cout << "Error opening the file";
	return 0; 
    }	
  
    // Read the Data from the file 
    // as String Vector 
    vector<string> row;
    string line, word; 
    int rowIndex = 0;
  
    while (getline(fin, line)) { 
  
        row.clear(); 
  
        // used for breaking words 
        stringstream s(line); 
  
        // read every column data of a row and 
        // store it in a string variable, 'word' 
        while (getline(s, word, ',')) { 
  
            // add all the column data 
            // of a row to a vector 
            row.push_back(word); 
        } 
         
        if(rowIndex > 0){
	  /*bypassing the titles*/
	  process(row);
  	}
        rowIndex++;
    } 

    //TBD: finalize(edge_list, node_list); -- sort node_ids, then use new IDs starting form 0
    //TBD: print_sorted(edge_list, out_file); -- sort edge_list in order of t, then print it to the output file

    fin.close();
    return 0;  
}   

void process(vector<string> row){
    /*Unify the time zone of departure time
    For now I use NY as the reference zone*/

    /*Add date to the departure time*/

    /*Add the row info to the edge list*/

    /*Keep track of the node IDs in node list*/
}
