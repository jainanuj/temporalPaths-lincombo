#include "graph.h"

Graph::Graph(const char* filePath)
{
    FILE* file = fopen(filePath,"r");
	int x;
   	x=fscanf(file, "%d %d",&V, &dynamic_E);
    
    //added by sanaz:
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

//added by sanaz: to remove the dominant edges from the edge list
void Graph::dominatedRemoval(){
    sort(edge_list.begin( ), edge_list.end( ), [ ](const Edge& e1, const Edge& e2){
	if(e1.u != e2.u) return e1.u < e2.u;
 	if(e1.v != e2.v) return e1.v < e2.v;
	if(e1.t != e2.t) return e1.t < e2.t;
	return e1.w > e2.w;
    }); 
    int listSize = edge_list.size();
    for(int i=0; i<listSize; i++){
	int j = i+1;
	while(j < listSize && edge_list[i].u == edge_list[j].u && edge_list[i].v == edge_list[j].v){
	     Edge e1 = edge_list[i];
	     Edge e2 = edge_list[j];
	     if((e1.t < e2.t && e1.t+e1.w >= e2.t+e2.w) || (e1.t == e2.t && e1.w >= e2.w)){
		 edge_list.erase(edge_list.begin()+i);
		 listSize--;
		 i--;
		 break;
	      }
	     j++;
	}
    }

    /*for debugging*/
    /*cout << "the edge_list after removing the dominant edges:" << endl;
    for(int i=0; i<edge_list.size(); i++)
	cout << "u: " << edge_list[i].u << ", v: " << edge_list[i].v << ", t: " << edge_list[i].t << ", w: " << edge_list[i].w << endl;*/

    return;
}

//added by sanaz
void Graph::transform(){
   vector<set<int>> Tout; //the set of distinct in times for each node
   Tout.resize(V);
   for(Edge e : edge_list){
	Tout[e.u].insert(e.t);
   }

   set <int>::iterator it; 
   //to map (u, t) to their corresponding IDs in the transformed graph
   map<pair<int, int>, int> outMap;
   int index = 0; 
   int t; 
   for(int i=0; i<V; i++){
	voutStart.push_back(index);
	Tout[i].insert(infinity); //so that nodes with empty Tout but non-empty Tin are not discarded
	for(it=Tout[i].begin(); it!=Tout[i].end(); it++){
	  t = *it;
          Node tmpNode(i, t); 
	  vertexList.push_back(tmpNode);
	  outMap[make_pair(i, t)] = index++;
	}
   }

   /*cout << "node_list:" << endl;
   for(int i=0; i<node_list.size(); i++)
	cout << "u: " << node_list[i].u << ", t: " << node_list[i].t << endl;*/

   //edge creation step:
   int edge_cnt = 0; //just for debugging
   for(Edge e : edge_list){
	int u_index = outMap[make_pair(e.u, e.t)];
	for(auto set_it = Tout[e.v].begin(); set_it != Tout[e.v].end(); set_it++){
	   t = *set_it;
	   if(t >= e.t+e.w){
	      int v_index = outMap[make_pair(e.v, t)];
	      vertexList[u_index].adjList.push_back(make_pair(v_index, e.w));
	      edge_cnt++; // just for debugging
	   }
	}
   }

   cerr << "number of edges after transform: " << edge_cnt << endl;
   cerr << "number of nodes after transform: " << index << endl;

   //filling up the reverse adjacency list:
   rev_adjList.resize(index);
   for(int i=0; i<index; i++){
	for(auto neigh=vertexList[i].adjList.begin(); neigh!=vertexList[i].adjList.end(); neigh++){
	    rev_adjList[neigh->first].push_back(make_pair(i, neigh->second)); 
        }
   }

   //for debugging:
   //print_adjList();
}

void Graph::print_adjList(){  
   cout << "(u1, t1) (u2, t2) W" << endl; 

   for(int i=0; i<vertexList.size(); i++){

     for(int j=0; j<vertexList[i].adjList.size(); j++){	

        //print out the source node 
	cout << "(" << vertexList[i].u << ", " << vertexList[i].t << ") "; 

	//print out the destination node
	int id = vertexList[i].adjList[j].first;
	int w = vertexList[i].adjList[j].second; 
        cout << "(" << vertexList[id].u << ", " << vertexList[id].t << ") ";
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

/*void Graph::run_earliest_arrival()
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
}*/

//modified by sanaz
/*void Graph::earliest_arrival(int source)
{
    Timer t;
    t.start();
	
    //define and initialize data structures	
    vector<bool> visited(vertexList.size(), false);
    queue<int> Q; 

    for(int i=vinStart[source]; i<vinStart[source+1]; i++){
	Q.push(i);
    }

    while(!Q.empty()){
	int node = Q.front(); 
	Q.pop();
	for(auto neighbor=vertexList[node].adjList.begin(); neighbor!=vertexList[node].adjList.end(); neighbor++){
	    //TO BE OPTIMIZED
	    //the time we enter the "source node" doesn't matter, but the time we exit it does
	    int exitTime = vertexList[neighbor->first].t - neighbor->second;
	    if(exitTime < t_start || exitTime > t_end)
		continue;
	    //UP TO HERE
	    int nID = neighbor->first; 
	    Node neiNode = vertexList[nID];
	    if(!visited[nID] && neiNode.t >= t_start && neiNode.t <= t_end){
		visited[nID] = true;
		Q.push(nID);
		if(neiNode.t < distances[neiNode.u]){
		   distances[neiNode.u] = neiNode.t; 
		}
	    }
	}
    }
	
    t.stop();
    time_sum += t.GetRuntime();

    //for debugging only
    for(int i=0; i<distances.size(); i++)
        cout << distances[i] << endl;
}*/
//-----------------

/*void Graph::run_latest_departure()
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
}*/

/*void Graph::latest_departure(int source)
{
    Timer t;
    t.start();
	
    //define and initialize data structures	
    vector<bool> visited(vertexList.size(), false); //nodes enqueued
    queue<int> Q;
    vector<set<int>> outSeen; //To be replaced by a more optimized DS
    outSeen.resize(vertexList.size()); 

    //initializing Q
    //put all the out times of source in outSeen[]
    for(int it = vinStart[source]; it < vinStart[source+1]; it++){
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end){
	   visited[it] = true; //it points to the index of the node
           Q.push(it);
        }
    } 

    while(!Q.empty()){
	int node = Q.front(); 
	Q.pop();
	for(auto neighbor=rev_adjList[node].begin(); neighbor!=rev_adjList[node].end(); neighbor++){
	    int nID = neighbor->first;
	    int exitTime = vertexList[node].t - neighbor->second; 
	    Node neiNode = vertexList[nID]; //eniNode.t is the in time
 	    //Vin node can be redundant, but its out time should not be already checked
	    if(neiNode.t >= t_start && neiNode.t <= t_end){
	        if(!visited[nID]){
  		   //otherwise, there is no point in putting a redundant Vin in Q
		   visited[nID] = true; 
		   Q.push(nID);
		}
		if(exitTime > distances[neiNode.u])
		   distances[neiNode.u] = exitTime; 
	    }
	}
    }
	
    t.stop();
    time_sum += t.GetRuntime();

    //for debugging only
    for(int i=0; i<distances.size(); i++)
        cout << distances[i] << endl;
}*/

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

    vector<bool> visited(vertexList.size(), false);
    queue<int> Q;
    for(int it=voutStart[source]; it<voutStart[source+1]; it++){
	visited[it] = true;
    }

    for(int it=voutStart[source+1]-1; it>=voutStart[source]; it--){
	int ts = vertexList[it].t; //start time
	Q.push(it);
	while(!Q.empty()){
	    int node = Q.front(); 
	    Q.pop();
	    for(auto neighbor=vertexList[node].adjList.begin(); neighbor!=vertexList[node].adjList.end(); neighbor++){
		int nID = neighbor->first; 	
		Node neiNode = vertexList[nID];
		int inTime = vertexList[node].t + neighbor->second;	
		if(inTime >= t_start && inTime <= t_end){
		   if(!visited[nID]){
		      visited[nID] = true; 
		      Q.push(nID);
		   }
		   if((inTime-ts) < distances[neiNode.u]){
		      distances[neiNode.u] = (inTime-ts); 
		   }
		}
	    }
	}
     }
	
    t.stop();
    time_sum += t.GetRuntime();

    //for debugging only
    for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl;
}
//-----------------

/*void Graph::run_shortest()
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

}*/

/*void Graph::shortest(int source)
{
    Timer t;
    t.start();

    //defining and initializing data structures
    typedef pair<int, int> iPair; 	
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq; //pairs of <distance, id> sorted in increasing order of distance
    vector<int> local_dist(vertexList.size(), infinity); //distance vector for the nodes in the transformed graph
    vector<bool> done(vertexList.size(), false); //keeping track of the nodes whose distance is established
    
    //for(auto it=Vin[source].begin(); it!=Vin[source].end(); it++){
    for(int it=vinStart[source]; it < vinStart[source+1]; it++){
        pq.push(make_pair(0, it));
	local_dist[it] = 0; 
    }

    while(!pq.empty()){
	int node = pq.top().second; 
	pq.pop(); 
	if(done[node]) 
	   continue; 
	done[node] = true; 
	for(auto neigh=vertexList[node].adjList.begin(); neigh!=vertexList[node].adjList.end(); neigh++){
	   int exitTime = vertexList[neigh->first].t - neigh->second;
	   if(exitTime < t_start || exitTime > t_end)
		continue;
	   int neiID = neigh->first;  
	   //vertexList[neiID].t represents "in time" of a Vin node
	   if(!done[neiID] && vertexList[neiID].t >= t_start && vertexList[neiID].t <= t_end){
		int newDist = local_dist[node]+neigh->second;
		if(newDist < local_dist[neiID]){
		   local_dist[neiID] = newDist; 
		   pq.push(make_pair(newDist, neiID));  
		}
	   }
  	}
    }

    for(int i=0; i<vertexList.size(); i++)
	if(vertexList[i].t >= t_start && vertexList[i].t <= t_end){
	   int u = vertexList[i].u; 
	   distances[u] = min(distances[u], local_dist[i]);
	}
	
    t.stop();
    time_sum += t.GetRuntime();

    //for debugging only
    for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl; 

}*/

void Graph::print_avg_time()
{
	cout<<"Average time: " << time_sum/numSources <<endl;
}
