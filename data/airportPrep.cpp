#include <iostream>
#include <vector>
#include <string>
#include <sstream> 
#include <fstream>
#include <time.h> /* time_t, struct tm, time, gmtime */
#include <cstddef>  /*std::size_t*/
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
    hh = stoi(row[4].substr(1,2));
    mm = stoi(row[4].substr(3,2));
    depStr.tm_hour = hh;
    depStr.tm_min = mm;

    /*also add year-month-day-daylight saving information
     Time zone differnce may alter the day*/
    string date = row[0]; //FL_DATE
    if(date.size() < 6) return; //invalid "date" string
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));
    depStr.tm_year = year; 
    depStr.tm_mon = month;
    depStr.tm_mday = day;
    cout << "year: " << year << ", month: " << month << ", day: " << day << endl; 
    depStr.tm_isdst = -1; //daylight saving info will be automatically inferred
     
    /*change the time zone environment variable*/
    string timezone = "America/"; //I suppose all the flights are internal
    string state = row[2].substr(1, row[2].size()-2); //ORIGIN_STATE_NM (it contains "" that should be omitted)
    
    /*preprocess the state names with two parts like New York*/
    size_t found = state.find_first_of(" ");    
    if(found != string::npos)
       state[found] = '_';
    timezone += state;

    cout << "timezone: " << timezone << endl;
    const char* cstr_TZ = timezone.c_str(); 
    if(setenv("TZ", cstr_TZ, 1) != 0)
       cout << "error in setting timezone!" << endl; 
  
    time_t depLocalTime = mktime(&depStr);

    //step2: converting local time to UTC time (time at the GMT timezone)
    struct tm* depUtcTime;
    depUtcTime = gmtime(&depLocalTime);
    cout << "UTC hh: " << depUtcTime->tm_hour << ", UTC mm: " << depUtcTime->tm_min << endl; 
    cout << "UTC year: " << depUtcTime->tm_year << ", UTC month: " << depUtcTime->tm_mon << ", UTC day: " << depUtcTime->tm_mday << endl; 

    /*Add date to the departure time*/

    /*Add the row info to the edge list*/

    /*Keep track of the node IDs in node list*/

    unsetenv("TZ");
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
