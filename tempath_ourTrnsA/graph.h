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

const int infinity = 1e9;
/*added by sanaz*/
int numSources; 

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
    Node() {}
    Node(int _u, int _t){
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

public:
    vector< Edge > edge_list;
    vector< int > sources;
    int V, static_E, dynamic_E;
    int t_start, t_end;
    double time_sum;
	//vector <int> arr_time, f_time;
	//vector < set < pair< int, int > > > ft_timepair; // arrival time, starting time
	//vector < set < pair< int, int > > > st_timepair; // arrival time, shortest distance 

    //added by sanaz
    vector<set<int>> Vin; //for each node u contains Vin(u) as a set of newIDs
    //vector<set<int>> Vout; //for each node u contains Vout(u) as a set of newIDs
    vector<vector<pair<int, int>>> outVec; //for each node in the original graph, cotains a list of <neighbor newID, exit time> (neighbor is a Vin node)
    vector<Node> node_list; //a list of 'Node's kept for reference
    vector<vector<pair<int, int>>> adj_list; //<node index(int the transformed graph) --> list<neighbor index, weight>> 
    vector<int> distances; 
    vector<vector<pair<int, int>>> rev_adjList; //adjacency list with reversed edges used in "latest" function
    //--------------
};

Graph::Graph(const char* filePath)
{
    FILE* file = fopen(filePath,"r");
	int x;
   	x=fscanf(file, "%d %d",&V, &dynamic_E);

	//arr_time.resize(V);
	//f_time.resize(V);
	//ft_timepair.resize(V);
	//st_timepair.resize(V);
    
    //added by sanaz:
    Vin.resize(V);
    //Vout.resize(V);
    outVec.resize(V);
    distances.resize(V); 
    //---------------
	
	Edge e; 
    for(int i = 0; i < dynamic_E; i ++)
    {
        x=fscanf(file, "%d %d %d %d",&e.u, &e.v, &e.t, &e.w);
        edge_list.push_back(e);
    }

    fclose(file);
}

//added by sanaz
void Graph::transform(){
   vector<set<int>> Tin; //the set of distinct in times for each node
   //vector<set<int>> Tout; //the set of distinct out times for each node
   Tin.resize(V);
   //Tout.resize(V); 
   for(Edge e : edge_list){
	//initially set all the newIDs (indices in the transformed graph) to -1
	Tin[e.v].insert(e.t+e.w);
	//Tout[e.u].insert(e.t);
   }

   set <int>::iterator it; 
   //to map (u, t) to their corresponding IDs in the transformed graph
   map<pair<int, int>, int> inMap;
   //map<pair<int, int>, int> outMap;
   int index = 0; 
   //Node tmpNode; 
   int t; 
   for(int i=0; i<V; i++){
	for(it=Tin[i].begin(); it!=Tin[i].end(); it++){
	  t = *it;
	  //tmpNode = new Node(i, t, true);
          Node tmpNode(i, t); 
	  node_list.push_back(tmpNode);
	  Vin[i].insert(index);
	  inMap[make_pair(i, t)] = index++;
	}
	//for(it=Tout[i].begin(); it!=Tout[i].end(); it++){
	  //t = *it;
	  //tmpNode = new Node(i, t, false);
	  //Node tmpNode(i, t, false); 
	  //node_list.push_back(tmpNode);
	  //Vout[i].insert(index);
	  //outMap[make_pair(i, t)] = index++;
	//}
   }

   //filling in the outVec vector:
   for(Edge e : edge_list){
	int neighID = inMap[make_pair(e.v, e.t+e.w)];
	outVec[e.u].push_back(make_pair(neighID, e.t));
   }

   adj_list.resize(index); 

   //edge creation step:
   for(Edge e : edge_list){
	int v_index = inMap[make_pair(e.v, e.t+e.w)];
	//for(t : Tin[e.u])
	for(auto set_it = Tin[e.u].begin(); set_it != Tin[e.u].end(); set_it++){
	   t = *set_it;
	   if(t <= e.t){
	      int u_index = inMap[make_pair(e.u, t)];
	      adj_list[u_index].push_back(make_pair(v_index, e.w));
	   }
	}
   }

   //filling up the reverse adjacency list:
   rev_adjList.resize(index);
   for(int i=0; i<index; i++){
	for(auto neigh=adj_list[i].begin(); neigh!=adj_list[i].end(); neigh++){
	    rev_adjList[neigh->first].push_back(make_pair(i, neigh->second)); 
        }
   }
}

void Graph::print_adjList(){  
   cout << "(u1, t1) (u2, t2) W" << endl; 

   for(int i=0; i<adj_list.size(); i++){

     for(int j=0; j<adj_list[i].size(); j++){	

        //print out the source node 
	cout << "(" << node_list[i].u << ", " << node_list[i].t << ", "; 

	//print out the destination node
	int id = adj_list[i][j].first;
	int w = adj_list[i][j].second; 
        cout << "(" << node_list[id].u << ", " << node_list[id].t << ", ";
	cout << w << endl; 
     }
   }
}
//--------------

void Graph::initial_query(int numS)
{
    t_start = 0;
    t_end = infinity;

    /*added by sanaz*/
    numSources = numS; 
	
    int s;
    for(int i = 0 ; i < numSources; i++)
    {
    	s=rand()%V;
        sources.push_back(s);
    }

}

void Graph::initial_query(const char* filePath, int numS)
{
    t_start = 0;
    t_end = infinity;

    /*added by sanaz*/
    numSources = numS; 
	
    FILE* file = fopen(filePath,"r");
    int s, x;
    for(int i = 0 ; i < numSources; i++)
    {
    	x=fscanf(file,"%d",&s);
    //	int y;
    //	x=fscanf(file,"%d%d",&s, &y);
        sources.push_back(s);
    }

}

void Graph::run_earliest_arrival()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size(); i ++)
    { 
    	//modified by sanaz:
	for(int j=0; j<V; j++)
	  distances[j] = infinity; 
	distances[sources[i]] = t_start; 
	//------------------
    	earliest_arrival(sources[i]);
    }
	
	print_avg_time();
}

//modified by sanaz
void Graph::earliest_arrival(int source)
{
    Timer t;
    t.start();
	
    /*define and initialize data structures*/	
    vector<bool> visited(node_list.size(), false);
    queue<int> Q; 

    /*initializing Q*/
    set<int>::iterator it; 
    for(it = Vin[source].begin(); it != Vin[source].end(); it++){
	visited[*it] = true; 
        Q.push(*it);
    }

    while(!Q.empty()){
	int node = Q.front(); 
	Q.pop();
	for(auto neighbor=adj_list[node].begin(); neighbor!=adj_list[node].end(); neighbor++){
	    /*TO BE OPTIMIZED*/
	    /*the time we enter the "source node" doesn't matter, but the time we exit it does*/
	    int exitTime = node_list[neighbor->first].t - neighbor->second;
	    if(exitTime < t_start || exitTime > t_end)
		continue;
	    /*UP TO HERE*/
	    int nID = neighbor->first; 
	    Node neiNode = node_list[nID];
	    if(!visited[nID] && neiNode.t >= t_start && neiNode.t <= t_end){
		visited[nID] = true; 
		Q.push(nID);
		if(neiNode.t < distances[neiNode.u])
		   distances[neiNode.u] = neiNode.t; 
	    }
	}
    }
	
    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl; 

}
//-----------------

void Graph::run_latest_departure()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//modified by sanaz:
	for(int j=0; j<V; j++)
	  distances[j] = t_start-1; 
	distances[sources[i]] = t_end; 
	//------------------
    	latest_departure(sources[i]);
    }
	
	print_avg_time();
}

void Graph::latest_departure(int source)
{
    Timer t;
    t.start();
	
    /*define and initialize data structures*/	
    vector<bool> visited(node_list.size(), false); //nodes enqueued
    queue<int> Q;
    vector<set<int>> outSeen;
    outSeen.resize(node_list.size()); 

    /*initializing Q*/
    /*put all the out times of source in outSeen[]*/
    set<int>::iterator it; 
    for(it = Vin[source].begin(); it != Vin[source].end(); it++){
	if(node_list[*it].t >= t_start && node_list[*it].t <= t_end){
	   visited[*it] = true; //it points to the index of the node
           Q.push(*it);
	   /*TO BE OPTIMIZED*/
	   for(auto link_it = adj_list[*it].begin(); link_it != adj_list[*it].end(); link_it++){
	       int exitTime = node_list[link_it->first].t - link_it->second;
	       outSeen[*it].insert(exitTime);
	   }
	   /*UP TO HERE*/
        }
    } 

    while(!Q.empty()){
	int node = Q.front(); 
	Q.pop();
	for(auto neighbor=rev_adjList[node].begin(); neighbor!=rev_adjList[node].end(); neighbor++){
	    int nID = neighbor->first;
	    int exitTime = node_list[node].t - neighbor->second; 
	    Node neiNode = node_list[nID]; //eniNode.t is the in time
 	    /*Vin node may be redundant, but its out time should not be already checked*/
	    if(neiNode.t >= t_start && neiNode.t <= t_end && outSeen[nID].find(exitTime) == outSeen[nID].end()){
	        if(!visited[nID]){
  		   /*otherwise, there is no point in putting a redundant Vin in Q*/
		   visited[nID] = true; 
		   Q.push(nID);
		}
		outSeen[nID].insert(exitTime);
		if(exitTime > distances[neiNode.u])
		   distances[neiNode.u] = exitTime; 
	    }
	}
    }
	
    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl; 

}

void Graph::run_fastest()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//modified by sanaz:
	for(int j=0; j<V; j++)
	  distances[j] = infinity; 
	distances[sources[i]] = 0; 
	//------------------
    	fastest(sources[i]);
    }
    
    print_avg_time();

}

//modified by sanaz
void Graph::fastest(int source)
{
    Timer t;
    t.start();
		
    /*define and initialize data structures*/	
    vector<pair<int, int>> startPoints;
    /*for(int i : Vout[source])
	if(node_list[i].t >= t_start && node_list[i].t <= t_end)
	  startPoints.push_back(make_pair(i, node_list[i].t)); */
    for(auto s_it = outVec[source].begin(); s_it != outVec[source].end(); s_it++){
	if(s_it->second >= t_start && s_it->second <= t_end)
	  startPoints.push_back(*s_it);
    }
    sort(startPoints.begin( ), startPoints.end( ), [ ](const pair<int, int>& p1, const pair<int, int>& p2){
       return p1.second > p2.second;
    });
    vector<bool> visited(node_list.size(), false);
    queue<int> Q; 

    //newly added: what if we get to Vin[source] at some point?
    for(auto s_it = Vin[source].begin(); s_it != Vin[source].end(); s_it++)
	visited[*s_it] = true;

    for(auto it = startPoints.begin(); it != startPoints.end(); it++){
	//int ts = node_list[it->first].t;
	int ts = it->second;

	//here it->first is a neighbor of the source
        visited[it->first] = true; 
        Q.push(it->first);
	int tmp_time =  node_list[it->first].t-ts;
	if(tmp_time < distances[it->first])
	   distances[it->first] = tmp_time;

	while(!Q.empty()){
	    int node = Q.front(); 
	    Q.pop();
	    for(auto neighbor=adj_list[node].begin(); neighbor!=adj_list[node].end(); neighbor++){
		int nID = neighbor->first; 
		Node neiNode = node_list[nID];
		if(!visited[nID] && neiNode.t >= t_start && neiNode.t <= t_end){
		   visited[nID] = true; 
		   Q.push(nID);
		   if((neiNode.t-ts) < distances[neiNode.u])
		      distances[neiNode.u] = (neiNode.t-ts); 
		}
	    }
	}
     }
	
    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl; 

}
//-----------------

void Graph::run_shortest()
{
    time_sum=0;
	
    for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//modified by sanaz:
	for(int j=0; j<V; j++)
	  distances[j] = infinity; 
	distances[sources[i]] = 0; 
    	shortest(sources[i]);
    }
    
    print_avg_time();

}

void Graph::shortest(int source)
{
    Timer t;
    t.start();

    /*defining and initializing data structures*/
    typedef pair<int, int> iPair; 	
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq; //pairs of <distance, id> sorted in increasing order of distance
    vector<int> local_dist(adj_list.size(), infinity); //distance vector for the nodes in the transformed graph
    vector<bool> done(adj_list.size(), false); //keeping track of the nodes whose distance is established
    
    for(auto it=Vin[source].begin(); it!=Vin[source].end(); it++){
        pq.push(make_pair(0, *it));
	local_dist[*it] = 0; 
    }

    while(!pq.empty()){
	int node = pq.top().second; 
	pq.pop(); 
	if(done[node]) 
	   continue; 
	done[node] = true; 
	for(auto neigh=adj_list[node].begin(); neigh!=adj_list[node].end(); neigh++){
	   /*TO BE OPTIMIZED*/
	   /*Vin[source] need not be in range, but the exit times should be*/
	   int exitTime = node_list[neigh->first].t - neigh->second;
	   if(exitTime < t_start || exitTime > t_end)
		continue;
	   /*UP TO HERE*/
	   int neiID = neigh->first;  
	   //node_list[neiID].t represents "in time" of a Vin node
	   if(!done[neiID] && node_list[neiID].t >= t_start && node_list[neiID].t <= t_end){
		int newDist = local_dist[node]+neigh->second;
		if(newDist < local_dist[neiID]){
		   local_dist[neiID] = newDist; 
		   pq.push(make_pair(newDist, neiID));  
		}
	   }
  	}
    }

    for(int i=0; i<adj_list.size(); i++)
	if(node_list[i].t >= t_start && node_list[i].t <= t_end){
	   int u = node_list[i].u; 
	   distances[u] = min(distances[u], local_dist[i]);
	}
	
    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl; 

}

void Graph::print_avg_time()
{
	cout<<"Average time: " << time_sum/numSources <<endl;
}

#endif

