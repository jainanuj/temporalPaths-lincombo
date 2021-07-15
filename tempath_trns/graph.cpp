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
	#ifdef USE_INT
        x=fscanf(file, "%d %d %d %d",&e.u, &e.v, &e.t, &e.w);
	#else
	x=fscanf(file, "%d %d %ld %d",&e.u, &e.v, &e.t, &e.w);
	#endif
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
    int i = listSize-1;
    while(i>=0){
	TTYPE minTW = edge_list[i].t+edge_list[i].w;
	int curU = edge_list[i].u;
	int curV = edge_list[i].v;
	while(--i>=0 && edge_list[i].u == curU && edge_list[i].v == curV ){
	     TTYPE tmpTW = edge_list[i].t + edge_list[i].w;
	     if(tmpTW >= minTW)
		edge_list.erase(edge_list.begin()+i);
	     else
		minTW = tmpTW;
	}
    }

    /*for debugging*/
    /*cout << "the edge_list after removing the dominant edges:" << endl;
    for(int i=0; i<edge_list.size(); i++)
	cout << "u: " << edge_list[i].u << ", v: " << edge_list[i].v << ", t: " << edge_list[i].t << ", w: " << edge_list[i].w << endl;*/
    return;
}

//added by sanaz
bool Graph::transform(){
   bool zeroW = false; //checks whether there are any links of w = 0 in the transformed graph

   vector<set<TTYPE>> Tin; //the set of distinct in times for each node
   vector<set<TTYPE>> Tout; //the set of distinct out times for each node
   Tin.resize(V);
   Tout.resize(V); 
   for(Edge e : edge_list){
	//initially set all the newIDs (indices in the transformed graph) to -1
	Tin[e.v].insert(e.t+e.w);
	Tout[e.u].insert(e.t);
   }

   set <TTYPE>::iterator it; 
   //two maps to map (u, t) in Tin/Tout to their corresponding IDs in the transformed graph
   map<pair<int, TTYPE>, int> inMap;
   map<pair<int, TTYPE>, int> outMap;
   int index = 0; 
   //Node tmpNode; 
   TTYPE t; 
   for(int i=0; i<V; i++){
	vinStart.push_back(index);
	for(it=Tin[i].begin(); it!=Tin[i].end(); it++){
	  t = *it;
          Node tmpNode(i, t, true); 
	  vertexList.push_back(tmpNode);
	  inMap[make_pair(i, t)] = index++;
	}
	voutStart.push_back(index);
	for(it=Tout[i].begin(); it!=Tout[i].end(); it++){
	  t = *it;
	  Node tmpNode(i, t, false); 
	  vertexList.push_back(tmpNode);
	  outMap[make_pair(i, t)] = index++;
	}
   } 
   
   int edgeCnt = 0; 


   //edge creation step a:
   set <TTYPE>::reverse_iterator rit;
   set <TTYPE>::iterator tmp_it;
   unordered_set<TTYPE> matched; //t value of Tout[i] nodes that are already matched
   for(int i=0; i<V; i++){ 
        matched.clear();
	//iterate over Tin[i] in descending order of t (reverse)
	for(rit = Tin[i].rbegin(); rit != Tin[i].rend(); rit++){ 
	   TTYPE tIn = *rit; 
	   tmp_it = Tout[i].lower_bound(tIn); //the first item that doesn't go before (t, newID)
           if(tmp_it == Tout[i].end()) 
		continue;
	   TTYPE tOut = *tmp_it; 
	   if(matched.find(tOut) == matched.end()){ 
		matched.insert(tOut);
	   	vertexList[inMap[make_pair(i, tIn)]].adjList.push_back(make_pair(outMap[make_pair(i, tOut)], 0));
	 	zeroW = true;
		edgeCnt++;
	   }
	}
   }
   
   //a dummy value, so that accessing vinStart[source+1] is not out-of-bound
   vinStart.push_back(index);


   //edge creation step b:
   set<TTYPE>::iterator it2; 
   for(int i=0; i<V; i++){
      it = it2 = Tin[i].begin(); 
      if(it2!=Tin[i].end()) it2++;
      while(it2 != Tin[i].end()){  
	   vertexList[inMap[make_pair(i, *it)]].adjList.push_back(make_pair(inMap[make_pair(i, *it2)], 0));
	   zeroW = true;
	   it++;
	   it2++;
	   edgeCnt++;
      }
      it = it2 = Tout[i].begin();
      if(it2!=Tout[i].end()) it2++;
      while(it2 != Tout[i].end()){ 
	   vertexList[outMap[make_pair(i, *it)]].adjList.push_back(make_pair(outMap[make_pair(i, *it2)], 0));
	   zeroW = true;
	   it++;
	   it2++;
	   edgeCnt++;
      }
   }


   //edge creation step c: 
   for(Edge e : edge_list){
	if(e.w == 0)
	   zeroW = true;
	vertexList[outMap[make_pair(e.u, e.t)]].adjList.push_back(make_pair(inMap[make_pair(e.v, e.t+e.w)], e.w));
	edgeCnt++;
   }

   //filling up the reverse adjacency list:
   rev_adjList.resize(index);
   for(int i=0; i<index; i++){
	for(auto neigh=vertexList[i].adjList.begin(); neigh!=vertexList[i].adjList.end(); neigh++){
	    rev_adjList[neigh->first].push_back(i); 
        }
   }
	
   cerr << "number of nodes after transformation: " << vertexList.size() << endl;
   cerr << "number of edges after transformation: " << edgeCnt << endl;

   return zeroW;
}

bool Graph::cycleDetector(){
   vector<bool> inStack(vertexList.size(), false);
   vector<bool> visited(vertexList.size(), false);
   for(int i=0; i<vertexList.size(); i++){
	if(!visited[i] && cycleRec(visited, inStack, i))
	  return true;
   }
   return false;
}

bool Graph::cycleRec(vector<bool>& visited, vector<bool>& inStack, int index){
   visited[index] = true;
   inStack[index] = true;
   for(int i=0; i<vertexList[index].adjList.size(); i++){
	int neigh = vertexList[index].adjList[i].first;
	if(inStack[neigh])
	   return true;
        if(!visited[neigh] && cycleRec(visited, inStack, neigh))
	   return true;
   }
   inStack[index] = false;
   return false;
}

void Graph::topologicalOrder(){
   int n = vertexList.size(); //ignore the last dummy node
   vector<bool> visited(n, false);
   vector<int> orderTmp;
   for(int i=0; i<n; i++){
        if(!visited[i])
           topologicalRec(visited, i, orderTmp);
   }
  for(int i=n-1; i>=0; i-- )
     tpOrdered.push_back(orderTmp[i]);
}

void Graph::topologicalRec(vector<bool>& visited, int index, vector<int>& orderTmp){
   visited[index] = true;
   for(int i=0; i<vertexList[index].adjList.size(); i++){
      int neigh = vertexList[index].adjList[i].first;
      if(!visited[neigh])
         topologicalRec(visited, neigh, orderTmp);
   }
   orderTmp.push_back(index);
}

void Graph::print_adjList(){  
   cout << "(u1, t1, Vin1/Vout1) (u2, t2, Vin2/Vout2) W" << endl; 

   for(int i=0; i<vertexList.size(); i++){

     for(int j=0; j<vertexList[i].adjList.size(); j++){	

        //print out the source node 
	cout << "(" << vertexList[i].u << ", " <<vertexList[i].t << ", "; 
	if(vertexList[i].isVin)
	   cout << "Vin) ";
	else 
	   cout << "Vout) ";

	//print out the destination node
	int id = vertexList[i].adjList[j].first;
	int w = vertexList[i].adjList[j].second; 
        cout << "(" << vertexList[id].u << ", " << vertexList[id].t << ", ";
	if(vertexList[id].isVin)
	   cout << "Vin) ";
	else 
	   cout << "Vout) ";
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
    vector<bool> visited(vertexList.size(), false);
    queue<int> Q; 

    /*initializing Q*/
    for(int it = voutStart[source]; it < vinStart[source+1]; it++){
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end){
	   visited[it] = true; //it->first: t, it->second: newID
           Q.push(it);
        }
    }

    while(!Q.empty()){
	int node = Q.front(); 
	Q.pop();
	for(auto neighbor=vertexList[node].adjList.begin(); neighbor!=vertexList[node].adjList.end(); neighbor++){
	    int nID = neighbor->first; 
	    Node neiNode = vertexList[nID];
	    if(!visited[nID] && neiNode.t >= t_start && neiNode.t <= t_end){
		visited[nID] = true; 
		Q.push(nID);
		if(neiNode.isVin == true && neiNode.t < distances[neiNode.u])
		   distances[neiNode.u] = neiNode.t; 
	    }
	}
    }
	
    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    //for(int i=0; i<distances.size(); i++)
	//cout << distances[i] << endl; 

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
    vector<bool> visited(vertexList.size(), false);
    queue<int> Q; 

    /*initializing Q*/
    for(int it = vinStart[source]; it < voutStart[source]; it++){
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end){
	   visited[it] = true; //it points to the index of the node
           Q.push(it);
        }
    }

    while(!Q.empty()){
	int node = Q.front(); 
	Q.pop();
	for(auto neighbor=rev_adjList[node].begin(); neighbor!=rev_adjList[node].end(); neighbor++){
	    int nID = *neighbor; 
	    Node neiNode = vertexList[nID];
	    if(!visited[nID] && neiNode.t >= t_start && neiNode.t <= t_end){
		visited[nID] = true; 
		Q.push(nID);
		if(neiNode.isVin == false && neiNode.t > distances[neiNode.u])
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

    for(int it=vinStart[source+1]-1; it>=voutStart[source]; it--){
        visited[it] = true;
	TTYPE ts = vertexList[it].t; 
	if(ts > t_end)
	   continue;
	if(ts<t_start)
	   break;
        Q.push(it);
	while(!Q.empty()){
	    int node = Q.front(); 
	    Q.pop();
	    for(auto neighbor=vertexList[node].adjList.begin(); neighbor!=vertexList[node].adjList.end(); neighbor++){
		int nID = neighbor->first; 
		Node neiNode = vertexList[nID];
		if(!visited[nID] && neiNode.t <= t_end){
		   visited[nID] = true; 
		   Q.push(nID);
		   if(neiNode.isVin == true && (neiNode.t-ts) < distances[neiNode.u])
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
    typedef pair<TTYPE, int> iPair; 	
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq; //pairs of <distance, id> sorted in increasing order of distance
    vector<TTYPE> local_dist(vertexList.size(), infinity); //distance vector for the nodes in the transformed graph
    vector<bool> done(vertexList.size(), false); //keeping track of the nodes whose distance is established
    
    for(int it = voutStart[source]; it < vinStart[source+1]; it++)
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end){
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
	   int neiID = neigh->first;  
	   if(!done[neiID] && vertexList[neiID].t >= t_start && vertexList[neiID].t <= t_end){
		TTYPE newDist = local_dist[node]+neigh->second;
		if(newDist < local_dist[neiID]){
		   local_dist[neiID] = newDist; 
		   pq.push(make_pair(newDist, neiID));  
		}
	   }
  	}
    }

    for(int i=0; i<vertexList.size(); i++)
	if(vertexList[i].isVin && vertexList[i].t >= t_start && vertexList[i].t <= t_end){
	   int u = vertexList[i].u; 
	   distances[u] = min(distances[u], local_dist[i]);
	}
	
    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl; 

}

//added by sanaz//
void Graph::run_minhop(bool isCyclic)
{
    time_sum=0;
	
    for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//modified by sanaz:
	for(int j=0; j<V; j++)
	  distances[j] = infinity; 
	distances[sources[i]] = 0; 
	if(isCyclic)
    	   minhop(sources[i]);
	else
	   minhop_acyclic(sources[i]);
    }
    
    print_avg_time();
}


void Graph::minhop(int source)
{
    Timer t;
    t.start();

    /*defining and initializing data structures*/
    typedef pair<TTYPE, int> iPair; 	
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq; //pairs of <distance, id> sorted in increasing order of distance
    vector<TTYPE> local_dist(vertexList.size(), infinity); //distance vector for the nodes in the transformed graph
    vector<bool> done(vertexList.size(), false); //keeping track of the nodes whose distance is established
    
    for(int it = voutStart[source]; it < vinStart[source+1]; it++){
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end){
	   pq.push(make_pair(0, it));
	   local_dist[it] = 0; 
	} 
    }
    while(!pq.empty()){
	int node = pq.top().second;
	pq.pop(); 
	if(done[node]) 
	   continue; 
	done[node] = true; 
	for(auto neigh=vertexList[node].adjList.begin(); neigh!=vertexList[node].adjList.end(); neigh++){
	   int neiID = neigh->first;  
	   if(!done[neiID] && vertexList[neiID].t >= t_start && vertexList[neiID].t <= t_end){
		int linkW = (neigh->second == 0) ? 0 : 1;
		TTYPE newDist = local_dist[node]+linkW;
		if(newDist < local_dist[neiID]){
		   local_dist[neiID] = newDist;
		   pq.push(make_pair(newDist, neiID));  
		}
	   }
  	}
    }

    for(int i=0; i<vertexList.size(); i++)
	if(vertexList[i].isVin && vertexList[i].t >= t_start && vertexList[i].t <= t_end){
	   int u = vertexList[i].u; 
	   distances[u] = min(distances[u], local_dist[i]);
	}
	
    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    /*for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl;*/
}

void Graph::minhop_acyclic(int source){
    Timer t;
    t.start();

    vector<TTYPE> localDist(vertexList.size(), infinity);
    for(int it = voutStart[source]; it < vinStart[source+1]; it++){
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end)
	   localDist[it] = 0;
    }

    /*process the nodes in topological order*/
    for(int i=0; i<vertexList.size(); i++){
	int index = tpOrdered[i];
  	for(int j=0; j<vertexList[index].adjList.size(); j++){
	    int neigh = vertexList[index].adjList[j].first;
	    int linkW = (vertexList[index].adjList[j].second == 0)? 0 : 1;
	    if(localDist[neigh] > localDist[index] + linkW)
		localDist[neigh] = localDist[index] + linkW;
	}
    }

    /*use localDist to compute distances*/
    for(int i=0; i<vertexList.size(); i++)
	if(vertexList[i].isVin && vertexList[i].t >= t_start && vertexList[i].t <= t_end){
	   int u = vertexList[i].u; 
	   distances[u] = min(distances[u], localDist[i]);
	}

    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    /*for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl;*/
}

//--------------//

void Graph::print_avg_time()
{
	cout<<"Average time: " << time_sum/numSources <<endl;
}
