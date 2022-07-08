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
#include <stack>
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
    bool transform(); 
    void print_adjList(); //used for debugging
    void dominatedRemoval(); //to remove the dominant edges from the edge list
    bool cycleDetector();
    bool cycleRec(vector<bool>& visited, vector<bool>& inStack, int index); //recursive function to find the cycles
    void topologicalOrder();
    void topologicalRec(vector<bool>& visited, int i, vector<int>& orderTmp); //the recursive function called inside topologicalOrder()
    //--------------
    void initial_query(const char* filePath, int numS); // query file, numS argument added by sanaz
    void initial_query(int numS); //numS argument added by sanaz 
    void earliest_arrival(int source);
    /*void run_latest_departure(); 
    void latest_departure(int source);*/ 
    void fastest(int source); 
    void shortest(int source);
    void print_avg_time();
    
    //added by sanaz:
    void run_minhop(bool isCyclic);
    void minhop(int source);
    void minhop_acyclic(int source);
    void run_shortest(bool isCyclic);
    void shortest_acyclic(int source);
    void run_fastest(bool isCyclic);
    void fastest_acyclic(int source);
    void run_earliest_arrival(bool isCyclic);    
    void earliest_acyclic(int source);

    void run_linear();
    void linear_combo(int source);
    bool feasible(int arr, int dep, int vertIndex);


public:
    vector< Edge > edge_list;
    vector< int > sources;
    int V, static_E, dynamic_E;
    TTYPE t_start, t_end;
    double time_sum;

    /*added by sanaz*/
    int numSources; 

    vector<TTYPE> opt_linCombo;
    //added by sanaz
    vector<Node> vertexList; //a list of vertices in the transformed graph, sorted by u
    vector<int> voutStart; //an index into the vertexList: Vout set for node i (in the transformed graph) is in vertexList[voutStart[i], voutStart[i+1]) 
    vector<TTYPE> distances; 
    vector<vector<pair<int, int>>> rev_adjList; //adjacency list with reversed edges used in "latest" function
    vector<int> tpOrdered; //keeps the (transformed) indices of the nodes in topological order
    vector<int> tpStart; //used for the modified version of acyclic minhop - topological order
    //--------------
};
#endif

