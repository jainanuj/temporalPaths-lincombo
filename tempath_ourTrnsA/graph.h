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

//added by sanaz: if max timestamp fits into int or long
#define USE_INT

#ifdef USE_INT
#define TTYPE int 
#else
#define TTYPE long
#endif


using namespace std;

#ifdef USE_INT
const int infinity = 2e9;
#else
const long infinity = 2e18;
#endif

struct Edge
{
    int u, v, w;
    TTYPE t; 
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
    TTYPE t; //the time stamp corresponding to Vin/Vout
    vector<pair<int, int>> adjList;
    Node() {}
    Node(int _u, TTYPE _t){
	u = _u; 
	t = _t; 
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
    void Zchoche_Node_Vertex_Count();//used to count number of nodes and vertices in TRG_Zchoche
    //--------------
    void initial_query(const char* filePath, int numS); // query file, numS argument added by sanaz
    void initial_query(int numS); //numS argument added by sanaz
    void run_fastest(); 
    void fastest(int source);
    /*void run_shortest(); 
    void shortest(int source);*/
    void print_avg_time();
    


public:
    vector< Edge > edge_list;
    vector< int > sources;
    int V, static_E, dynamic_E;
    TTYPE t_start, t_end;
    double time_sum;
	//vector <int> arr_time, f_time;
	//vector < set < pair< int, int > > > ft_timepair; // arrival time, starting time
	//vector < set < pair< int, int > > > st_timepair; // arrival time, shortest distance 

    /*added by sanaz*/
    int numSources; 

    //added by sanaz
    vector<Node> vertexList; //a list of vertices in the transformed graph, sorted by u
    vector<int> voutStart; //an index into the vertexList: Vin set for node i (in the transformed graph) is in vertexList[voutStart[i], voutStart[i+1]) 
    vector<TTYPE> distances; 
    vector<vector<pair<int, int>>> rev_adjList; //adjacency list with reversed edges used in "latest" function
    //--------------
};
#endif

