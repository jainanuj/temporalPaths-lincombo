#include <iostream>
#include <vector>
#include <string>
#include <sstream> 
#include <fstream>
#include <time.h> /* time_t, struct tm, time, gmtime */
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
void printRow(vector<string> row);

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
        stringstream ss(line); 
  
        // read every column data of a row and 
        // store it in a string variable, 'word' 
        while (getline(ss, word, ',')) { 
  
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
    cout << "at the beginning of the process funciton" << endl;
    printRow(row); 
    /*return if number of diverted airport
    landings (DIV_AIRPORT_LANDINGS) is > 0*/
    if(row[6].size() < 1 || stoi(row[6]) > 0) return;     

    /*Unify the time zone of departure time
    For now I use NY as the reference zone*/
    
    //step1: converting string times (DEP_TIME) to actual date objects
    /*the time string is in the form "hhmm" (quotations are part of the string)*/
    if(row[4].size() < 6) return; //not valid time string
    int hh, mm;
    struct tm depStr = {0};
    //cout << "row[4]: " << row[4] << endl;
    string hhStr = row[4].substr(1,2);
    cout << "hhStr: " << hhStr << endl;
    string mmStr = row[4].substr(3,2);
    cout << "mmStr: " << mmStr << endl; 
    hh = stoi(row[4].substr(1,2));
    mm = stoi(row[4].substr(3,2));
    depStr.tm_hour = hh;
    depStr.tm_min = mm;
    cout << "hh: " << hh << ", mm: " << mm << endl; 
    time_t depLocalTime = mktime(&depStr);

    //step2: converting local time to UTC time (time at the GMT timezone)
    struct tm* depUtcTime;
    depUtcTime = gmtime(&depLocalTime);
    cout << "UTC hh: " << depUtcTime->tm_hour << ", UTC mm: " << depUtcTime->tm_min << endl; 

    /*Add date to the departure time*/

    /*Add the row info to the edge list*/

    /*Keep track of the node IDs in node list*/
}

//Just for debugging
void printRow(vector<string> row){
    cout << "FL_DATE: " << row[0];
    cout << ", ORIGIN_AIRPORT_ID: " << row[1]; 
    cout << ", ORIGIN_STATE_NM: " << row[2]; 
    cout << ", DEST_AIRPORT_ID: " << row[3]; 
    cout << ", DEP_TIME: " << row[4]; 
    cout << ", ACTUAL_ELAPSED_TIME: " << row[5]; 
    cout << ", DIV_AIRPORT_LANDINGS: " << row[6] << endl; 
}
