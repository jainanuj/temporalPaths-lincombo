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
  int u, v, t, w; 
  Edge(int _u, int _v, int _t, int _w){
	u = _u; 
	v = _v; 
	t = _t; 
	w = _w; 
  }
};

struct edgeSort {
  bool operator() (Edge e1, Edge e2) { return (e1.t < e2.t);}
} eSort;

unordered_map<string, string> zoneMap;
vector<Edge> edgeList;
unordered_map<int, int> nodeMap;

void process(vector<string> row);
void printRow(vector<string> row);

int main(int argc, char *argv[]) 
{  
    if(argc < 2){
	cout << "Please enter the path to the output file" << endl; 
	return 0; 
    }    

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

    /*sort the edgeList in increasing order of t
    Re-number nodes from 0*/
    sort(edgeList.begin(), edgeList.end(), eSort);

    unordered_map<int, int>::iterator it; 
    int newID = 0; 
    for(int i=0; i<edgeList.size(); i++){
	//first deal with u
	it = nodeMap.find(edgeList[i].u);
	if(it == nodeMap.end()){
	   nodeMap[edgeList[i].u] = newID;
	   edgeList[i].u = newID++;
	}else{
	   edgeList[i].u = nodeMap[edgeList[i].u];
	}

	//now deal with v
	it = nodeMap.find(edgeList[i].v);
	if(it == nodeMap.end()){
	   nodeMap[edgeList[i].v] = newID;
	   edgeList[i].v = newID++;
	}else{
	   edgeList[i].v = nodeMap[edgeList[i].v];
	}
    }

    /*now write the edgelist to an output file*/
    ofstream outfile;
    outfile.open(argv[1]);
    if(!outfile.is_open())
	cout << "error in opening the outfile" << endl;

    outfile << newID << " " << edgeList.size() << endl;
    for(int i=0; i<edgeList.size(); i++)
	outfile << edgeList[i].u << " " << edgeList[i].v << " " << edgeList[i].t << " " << edgeList[i].w << endl; 

    fin.close();
    return 0;  
}   

void process(vector<string> row){
    //printRow(row); 
 
    /*return if number of diverted airport
    landings (DIV_AIRPORT_LANDINGS) is > 0 or if the data is missing*/
    if(row[6].size() < 1 || stoi(row[6]) > 0) return; 

    /*remove the other cases of missing data*/
    if(row[4].size() < 6) return; //not valid departure time string
    if(row[0].size() < 6) return; //invalid "date" string
    if(row[1].size() < 1 || row[2].size() <=2 || row[3].size() < 1 || row[5].size() < 1) return; 

    /*part A - Unify the time zone of departure time
    For now I use UTC as the reference zone*/
    
    //step1: converting string times (DEP_TIME) to actual date objectsv    
    /*the departure time string is in the form "hhmm" (quotations are part of the string)*/
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
    int year = stoi(date.substr(0, 4))-1900;
    int month = stoi(date.substr(5, 2))-1;
    int day = stoi(date.substr(8, 2));
    depStr.tm_year = year; 
    depStr.tm_mon = month;
    depStr.tm_mday = day; 
    depStr.tm_isdst = -1; //daylight saving info will be automatically inferred
     
    /*change the time zone environment variable*/
    /*assuming all flights are US internal*/
    string state = row[2].substr(1, row[2].size()-2); //ORIGIN_STATE_NM (it contains "" that should be omitted)
    string timezone = zoneMap[state];
    
    const char* cstr_TZ = timezone.c_str(); //the input to setenv() is const cstr* not string
    if(setenv("TZ", cstr_TZ, 1) != 0)
       cout << "error in setting timezone!" << endl; 
  
    time_t depLocalTime = mktime(&depStr); 
    /*the function ctime() converts time_t* to a human readable string */
    //cout << "the result of turning tm to time_t: " << ctime(&depLocalTime) << endl;

    unsetenv("TZ");

    //step2: converting local time to UTC time (time at the GMT timezone)
    struct tm* depUtcTime;
    depUtcTime = gmtime(&depLocalTime);
    /*note: important information contained in lines below. Most importantly: 
    the tm structure contains month-1 and year-1900. Them maketime() function
    adds 1 and 1900 to the corresponding values*/
    /*cout << "UTC hh: " << depUtcTime->tm_hour << ", UTC mm: " << depUtcTime->tm_min; 
    cout << ", UTC year: " << depUtcTime->tm_year+1900 << ", UTC month: " << depUtcTime->tm_mon+1 << ", UTC day: " << depUtcTime->tm_mday << endl; 
    time_t utc_time_t = mktime(depUtcTime);
    cout << "UTC time: " << ctime(&utc_time_t) << endl;*/

    /*part B - Add date to the departure time*/
    long timeStamp = depUtcTime->tm_min + 60*depUtcTime->tm_hour;
    int yDay = depUtcTime->tm_yday; //number of days since Jan 1st of the same year
    timeStamp += yDay*20*60; // in minutes 

    /*Add the row info to the edge list*/
    //I take ORIGIN_AIRPORT_ID and DEST_AIRPORT_ID as u and v respectively
    int u, v, w; 
    u = stoi(row[1]);
    v = stoi(row[3]);
    w = stoi(row[5]);
    Edge eTmp(u, v, timeStamp, w);
    edgeList.push_back(eTmp);
    
    return;
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
