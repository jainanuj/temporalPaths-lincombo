#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream> 
#include <fstream>
#include <time.h> /* time_t, struct tm, time, gmtime */
#include <cstddef>  /*std::size_t*/
#include <unordered_map>
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

unordered_map<string, string> zoneMap;

void process(vector<string> row);
void printRow(vector<string> row);

int main() 
{  

/*struct tm calTM; 
calTM.tm_hour = 9;
calTM.tm_min = 0;   
calTM.tm_year = 2021-1900; 
calTM.tm_mon = 1-1;
calTM.tm_mday = 1;
calTM.tm_isdst = -1;

setenv("TZ", "America/California", 1);
time_t calTime = mktime(&calTM);
unsetenv("TZ");

struct tm* utcTime;
utcTime = gmtime(&calTime);
cout << "UTC hh: " << utcTime->tm_hour << ", UTC mm: " << utcTime->tm_min; 
cout << ", UTC year: " << utcTime->tm_year << ", UTC month: " << utcTime->tm_mon << ", UTC day: " << utcTime->tm_mday << endl;
 
time_t utc_time_t = mktime(utcTime);
cout << "UTC transformed time: " << ctime(&utc_time_t) << endl;
return 0; */

    /*first deal with the timeZone map*/
    // File pointer 
    fstream fin; 
    fin.open("stateTimeZones.csv", ios::in);
    if(!fin.is_open()){
	cout << "Error opening the time zone file";
	return 0; 
    }	
   

    string line, word, nameStr, zoneStr;     
    getline(fin, line); //to bypass the titles line

    while (getline(fin, line)) { 
        // used for breaking words 
        stringstream ss(line);   
        getline(ss, nameStr, ','); //read the name of the state
        getline(ss, zoneStr, ','); //read the time zone of the state
        cout << "nameStr: " << nameStr << ", zoneStr: " << zoneStr << endl; 
        zoneMap[nameStr] = zoneStr;
    }     

    fin.close();
   
    /*now deal with the airport data*/
    // Open an existing file 
    fin.open("./AirportRawData/jan2020.csv", ios::in); 
    if(!fin.is_open()){
	cout << "Error opening the file";
	return 0; 
    }	  

    // Read the Data from the file 
    // as String Vector 
    vector<string> row;
 
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
    landings (DIV_AIRPORT_LANDINGS) is > 0 or if the data is missing*/
    if(row[6].size() < 1 || stoi(row[6]) > 0) return;     

    /*Unify the time zone of departure time
    For now I use UTC as the reference zone*/
    
    //step1: converting string times (DEP_TIME) to actual date objects
    /*the time string is in the form "hhmm" (quotations are part of the string)*/
    if(row[4].size() < 6) return; //not valid time string
    int hh, mm;
    struct tm depStr = {0};
    //memset(&depStr, 0, sizeof(tm));
    hh = stoi(row[4].substr(1,2));
    mm = stoi(row[4].substr(3,2));
    depStr.tm_hour = hh;
    depStr.tm_min = mm;

    /*also add year-month-day-daylight saving information
     Time zone differnce may alter the day*/
    string date = row[0]; //FL_DATE
    if(date.size() < 6) return; //invalid "date" string
    int year = stoi(date.substr(0, 4))-1900;
    int month = stoi(date.substr(5, 2))-1;
    int day = stoi(date.substr(8, 2));
    depStr.tm_year = year; 
    depStr.tm_mon = month;
    depStr.tm_mday = day;
    //cout << "year: " << year << ", month: " << month << ", day: " << day << ", hh: " << depStr.tm_hour << ", mm: " << depStr.tm_min << endl; 
    depStr.tm_isdst = -1; //daylight saving info will be automatically inferred
     
    /*change the time zone environment variable*/
    /*assuming all flights are US internal*/
    string state = row[2].substr(1, row[2].size()-2); //ORIGIN_STATE_NM (it contains "" that should be omitted)
    string timezone = zoneMap[state];
    
    //cout << "timezone: " << timezone << endl;
    const char* cstr_TZ = timezone.c_str(); //the input to setenv() is const cstr* not string
    if(setenv("TZ", cstr_TZ, 1) != 0)
       cout << "error in setting timezone!" << endl; 
  
    time_t depLocalTime = mktime(&depStr);
 
    cout << "the result of turning tm to time_t: " << ctime(&depLocalTime) << endl;

    unsetenv("TZ");

    //step2: converting local time to UTC time (time at the GMT timezone)
    struct tm* depUtcTime;
    depUtcTime = gmtime(&depLocalTime);
    //cout << "UTC hh: " << depUtcTime->tm_hour << ", UTC mm: " << depUtcTime->tm_min; 
    //cout << ", UTC year: " << depUtcTime->tm_year << ", UTC month: " << depUtcTime->tm_mon << ", UTC day: " << depUtcTime->tm_mday << endl; 
    time_t utc_time_t = mktime(depUtcTime);
    cout << "UTC time: " << ctime(&utc_time_t) << endl; 

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
