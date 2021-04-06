#ifndef GRAPH_H_HHWU
#define GRAPH_H_HHWU

#include <cstdio>
#include <vector>
#include <cstring>
#include <algorithm>
#include <set>
#include <queue>
#include <iostream>
//added by sanaz
#include <map>
#include <unordered_set>
#include <utility> //for std::pair 
#include <queue>
//--------------
#include "Timer.h"

using namespace std;

const int infinity = 2e9;

struct Edge
{
    int u, v, t, w;
    bool operator < (const Edge that) const
    {
        if (t != that.t)
            return t < that.t;
        if (u != that.u)
        	return u < that.u;
        if (v != that.v)
        	return v < that.v;
        return w < that.w;  
    }
};

//added by sanaz
struct Node{
    int u; //the node id in the non-transformed graph
    int t; //the time stamp corresponding to Vin/Vout
    vector<pair<int, int>> adjList;
    bool isVin; //true if this node belongs to Vin(u) and false if it belongs to Vout(u)
    Node() {}
    Node(int _u, int _t, bool _isVin){
	u = _u; 
	t = _t; 
	isVin = _isVin;
    }
};

//--------------

class Graph
{
public:
    Graph() {}
    Graph(const char* filePath); // input file
    //added by sanaz
    void transform(); 
    void print_adjList(); //used for debugging
    void dominatedRemoval(); //to remove the dominant edges from the edge list
    //--------------
    void initial_query(const char* filePath, int numS); // query file, numS argument added by sanaz
    void initial_query(int numS); //numS argument added by sanaz
    /*void initial_ds_ea();
    void initial_ds_ld();
    void initial_ds_f();
    void initial_ds_s();*/
    void run_earliest_arrival(); 
    void earliest_arrival(int source);
    void run_latest_departure(); 
    void latest_departure(int source);
    void run_fastest(); 
    void fastest(int source);
    void run_shortest(); 
    void shortest(int source);
    void print_avg_time();
    
    // for testing the correctness
    /*void run_earliest_arrival(const char* filePath); // output the result
    void earliest_arrival(int source, FILE * file);  
    void run_latest_departure(const char* filePath); 
    void latest_departure(int source, FILE * file); 
    void run_fastest(const char* filePath);
    void fastest(int source, FILE * file);
    void run_shortest(const char* filePath);
    void shortest(int source, FILE * file);
    void print_result(const int source, const vector<int>& t_time, FILE * file);
    void print_result_ld(const int source, const vector<int>& t_time, FILE * file); 
    void print_avg_time(const char* filePath1, const char* filePath2);*/

    //added by sanaz:
    void run_minhop();
    void minhop(int source);

public:
    vector< Edge > edge_list;
    vector< int > sources;
    int V, static_E, dynamic_E;
    int t_start, t_end;
    double time_sum;
	//vector <int> arr_time, f_time;
	//vector < set < pair< int, int > > > ft_timepair; // arrival time, starting time
	//vector < set < pair< int, int > > > st_timepair; // arrival time, shortest distance 


    /*added by sanaz*/
    int numSources; 

    //added by sanaz
    vector<Node> vertexList; //a list of vertices in the transformed graph, sorted by u
    vector<int> vinStart; //an index into the vertexList: Vin set for node i (in the transformed graph) is in vertexList[vinStart[i], voutStart[i])
    vector<int> voutStart; //an index into the vertexList: Vin set for node i (in the transformed graph) is in vertexList[voutStart[i], vinStart[i+1]) 
    vector<int> distances; 
    vector<vector<int>> rev_adjList; //adjacency list with reversed edges used in "latest" function
    //--------------
};

#endif

