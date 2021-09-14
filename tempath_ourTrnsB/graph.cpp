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

    return;
}

//added by sanaz
bool Graph::transform(){
   bool zeroW = false; //checks whether there are any links of w = 0 in the transformed graph

   vector<set<TTYPE>> Tout; //the set of distinct out times for each node
   Tout.resize(V);
   vector<TTYPE> maxIN(V, -1); //max in-time for each node
   vector<TTYPE> maxOUT(V, -1); //max out-time for each node
   for(Edge e : edge_list){
	Tout[e.u].insert(e.t);
        maxOUT[e.u] = max(maxOUT[e.u], e.t);
	maxIN[e.v] = max(maxIN[e.v], e.t+e.w);
   }

   set <TTYPE>::iterator it; 
   //to map (u, t) to their corresponding IDs in the transformed graph
   map<pair<int, TTYPE>, int> outMap;
   int index = 0; 
   TTYPE t; 
   for(int i=0; i<V; i++){
	voutStart.push_back(index);
	//dummy node, so that the nodes with an empty Tout but non-empty Tin or nodes whose lost input cannot get out are accounted for
	if(Tout[i].empty() || maxOUT[i] < maxIN[i])
	   Tout[i].insert(maxIN[i]);
	//elements in Tin[i] are sorted in increasing order of t
	for(it=Tout[i].begin(); it!=Tout[i].end(); it++){
	  t = *it;
          Node tmpNode(i, t); 
	  vertexList.push_back(tmpNode);
	  outMap[make_pair(i, t)] = index++;
	}
   }
   voutStart.push_back(index); //so that we can use voutStart[i]..voutStart[i+1] for the last node

   /*cout << "vertexList:" << endl;
   for(int i=0; i<vertexList.size(); i++)
	cout << "u: " << vertexList[i].u << ", t: " << vertexList[i].t << endl;*/

   //edge creation step 1: adding the weighted links
   int edge_cnt = 0;
   for(Edge e : edge_list){
	int u_index = outMap[make_pair(e.u, e.t)];
        //we are looking for the smallest t greater that e.t+e.w
	for(auto set_it = Tout[e.v].begin(); set_it != Tout[e.v].end(); set_it++){
	   t = *set_it;
	   if(t >= e.t+e.w){
	      int v_index = outMap[make_pair(e.v, t)];
	      vertexList[u_index].adjList.push_back(make_pair(v_index, e.w));
	      if(e.w == 0)
		 zeroW = true;
	      edge_cnt++; // just for debugging
	      break;
	   }
	}
   }

   //adding a dummy node to be used in minhop 
   Node tmp(-1, infinity);
   vertexList.push_back(tmp);

   cerr << "number of nodes after transform: " << index << endl;
   cerr << "number of edges after transform: " << edge_cnt << endl;

   //filling up the reverse adjacency list:
   rev_adjList.resize(index);
   for(int i=0; i<index; i++){
	for(auto neigh=vertexList[i].adjList.begin(); neigh!=vertexList[i].adjList.end(); neigh++){
	    rev_adjList[neigh->first].push_back(make_pair(i, neigh->second)); 
        }
   }

   return zeroW;
}

bool Graph::cycleDetector(){
   vector<bool> inStack(vertexList.size(), false);
   vector<bool> visited(vertexList.size(), false);
   for(int i=0; i<vertexList.size()-1; i++){ //ignore the last dummy node
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
   int n = vertexList.size()-1; //ignore the last dummy node
   vector<bool> visited(n, false);
   vector<int> orderTmp;
   for(int i=0; i<n; i++){
	if(!visited[i])
	   topologicalRec(visited, i, orderTmp);
   }
  tpStart.resize(n, -1); //modified version
  for(int i=n-1; i>=0; i-- ){
     tpOrdered.push_back(orderTmp[i]);
     int u = vertexList[orderTmp[i]].u;
     if(tpStart[u] == -1)
	tpStart[u] = tpOrdered.size()-1;
  }
}

void Graph::topologicalRec(vector<bool>& visited, int index, vector<int>& orderTmp){
   visited[index] = true;
   int u = vertexList[index].u;
   //take care of the next chain neighbor
   if(index+1 < voutStart[u+1] && !visited[index+1])
	topologicalRec(visited, index+1, orderTmp);
   for(int i=0; i<vertexList[index].adjList.size(); i++){
	int neigh = vertexList[index].adjList[i].first;
	if(!visited[neigh])
	   topologicalRec(visited, neigh, orderTmp);
   }
   orderTmp.push_back(index);
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

    //initializing Q
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
    stack<int> st;

    /*Our big assumption is that all edge weights are >= 0*/
    for(int it=voutStart[source+1]-1; it>=voutStart[source]; it--){
	visited[it] = true;
	TTYPE ts = vertexList[it].t; //start time
	if(ts > t_end)
	   continue;
	if(ts < t_start)
	  break;
	st.push(it);
	while(!st.empty()){
	    int node = st.top(); 
	    st.pop();
	    for(auto neighbor=vertexList[node].adjList.begin(); neighbor!=vertexList[node].adjList.end(); neighbor++){
		int nID = neighbor->first; 	
		Node neiNode = vertexList[nID];
		TTYPE inTime = vertexList[node].t + neighbor->second;
		if(inTime <= t_end){
		   if((inTime-ts) < distances[neiNode.u])
			 distances[neiNode.u] = (inTime-ts);
                }
		if(!visited[nID]){
		   visited[nID] = true;
		   if(vertexList[nID].t <= t_end){
		   	st.push(nID);		      
		        for(int it2=nID+1; it2 <voutStart[neiNode.u+1]; it2++){
		            if(visited[it2] || vertexList[it2].t > t_end)
			       break;
		            visited[it2] = true;
		            st.push(it2);
		        }
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

void Graph::run_shortest(bool isCyclic)
{
    time_sum=0;
	
    for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//modified by sanaz:
	for(int j=0; j<V; j++)
	  distances[j] = infinity; 
	distances[sources[i]] = 0; 
	if(isCyclic)
    	   shortest(sources[i]);
	else
	   shortest_acyclic(sources[i]);
    }
    
    print_avg_time();

}

void Graph::shortest(int source)
{
    Timer t;
    t.start();

    //defining and initializing data structures
    typedef pair<int, int> iPair; 	
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq; //pairs of <distance, id> sorted in increasing order of distance
    vector<int> local_dist(vertexList.size(), infinity); //distance vector for the nodes in the transformed graph
    vector<bool> done(vertexList.size(), false); //keeping track of the nodes whose distance is established
    vector<TTYPE> firstDone(vertexList.size(), infinity); //the time stamp of the last chain neighbor that has become Done
    
    for(int it=voutStart[source]; it < voutStart[source+1]; it++)
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end){
	   pq.push(make_pair(0, it));
	   local_dist[it] = 0; 
	   break;
    	}

    while(!pq.empty()){
	int node = pq.top().second; 
	pq.pop(); 
	if(done[node]) 
	   continue;  
        //first go through all its chain neighbors upto firstDone[node]
	int u = vertexList[node].u;
        for(int cNeigh = node; cNeigh < voutStart[u+1] && vertexList[cNeigh].t < firstDone[u] && vertexList[cNeigh].t <= t_end; cNeigh++){
	    //we also include the node itself to avoid redundant loops 
	    local_dist[cNeigh] = local_dist[node];
	    done[cNeigh] = true;
	    //now go through their neighbors:
	    for(auto neigh=vertexList[cNeigh].adjList.begin(); neigh!=vertexList[cNeigh].adjList.end(); neigh++){
		int neiID = neigh->first;
		int linkW = neigh->second;
		if(vertexList[neiID].t <= t_end && local_dist[node]+linkW < local_dist[neiID]){
		    local_dist[neiID] = local_dist[node]+linkW;
		    pq.push(make_pair(local_dist[neiID], neiID));
		}
	    }
	}
	firstDone[u] = vertexList[node].t;
    }

    for(int i=0; i<vertexList.size(); i++)
	if(vertexList[i].t >= t_start && vertexList[i].t <= t_end){
	   int u = vertexList[i].u; 
	   distances[u] = min(distances[u], local_dist[i]);
	}
	
    t.stop();
    time_sum += t.GetRuntime();

    //for debugging only
    /*for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl;*/ 

}

//added by sanaz
void Graph::shortest_acyclic(int source){
    Timer t;
    t.start();

    vector<TTYPE> localDist(vertexList.size(), infinity);
    for(int it = voutStart[source]; it < voutStart[source+1]; it++){
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end)
	   localDist[it] = 0;
    }
    
    for(int i=tpStart[source]; i<tpOrdered.size(); i++){
	int index = tpOrdered[i];
	if(vertexList[index].t > t_end || localDist[index] == infinity) //the modified version
	   continue;	
	int u = vertexList[index].u;
	distances[u] = min(distances[u], localDist[index]); //the modified version
	//first, take care of the next chain neighbor
	if(index+1 < voutStart[u+1] && vertexList[index+1].t <= t_end)
	   localDist[index+1] = min(localDist[index+1], localDist[index]);
	//now, take care of the other neighbors
	for(int j=0; j<vertexList[index].adjList.size(); j++){
	   int neigh = vertexList[index].adjList[j].first;
	   int linkW = vertexList[index].adjList[j].second;
	   if(vertexList[neigh].t > t_end)
		continue;
	   localDist[neigh] = min(localDist[index]+linkW, localDist[neigh]);
	}	   
    }

    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    /*for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl;*/
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

    vector<bool> live(V, false);
    vector<bool> newLive(V, false);
    int numVertices = vertexList.size()-1; 
    vector<int> eout(V, numVertices); //use indices instead of actual times - optimize loop for uu
    vector<int> newEout(V, numVertices); //added to resolve the bug
    vector<int> uniqueV; //used in companion with newEout
    vector<bool> visited(numVertices, false);
    vector<int> Q; //used vector instead of queue for optimization

    //Q.push(source);
    Q.push_back(source);
    eout[source] = voutStart[source];//t_start;
    newEout[source] = voutStart[source];//t_start; 
    live[source] = true;
    int hopCount = 1;
    int count = 1; 

    while(Q.size() > 0){
	for(int i=0; i<Q.size(); i++){
	    int u = Q[i];
	    live[u] = false;
	    //try all feasible touts from u not tried before 
	    for(int uu = eout[u]; uu < voutStart[u+1] && vertexList[uu].t <= t_end && !visited[uu]; uu++){
		visited[uu] = true;
		vector<pair<int, int>> x = vertexList[uu].adjList;
		for(int it=0; it<x.size(); it++){
		    int neigh = x[it].first; //new index
		    TTYPE tOut = vertexList[neigh].t;
		    int vv = vertexList[neigh].u; //old index
		    if(tOut < vertexList[newEout[vv]].t){ 
			if(newEout[vv] == numVertices){ //vv is newly reached
			   distances[vv] = hopCount;
			   if(++count == V){ //all vertices reached
				t.stop();
				time_sum += t.GetRuntime();
				//for debugging only
				for(int i=0; i<distances.size(); i++)
				    cout << distances[i] << endl;
				return;
			   }
			}//done
			newEout[vv] = neigh;
			if(!newLive[vv]){ 
			   newLive[vv] = true;
			   uniqueV.push_back(vv); //added to resolve the bug
			}
		    }
		}
	    }
	}
	Q.clear();
	for(int ii=0; ii<uniqueV.size(); ii++){
	    int index = uniqueV[ii];
	    eout[index] = newEout[index];
	    Q.push_back(index);
	}
	uniqueV.clear();
	swap(live, newLive);
	hopCount++;
    }        

    t.stop();
    time_sum += t.GetRuntime();

    //for debugging only
    /*for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl;*/
}

void Graph::minhop_acyclic(int source){
    Timer t;
    t.start();

    vector<TTYPE> localDist(vertexList.size(), infinity);
    for(int it = voutStart[source]; it < voutStart[source+1]; it++){
	if(vertexList[it].t >= t_start && vertexList[it].t <= t_end)
	   localDist[it] = 0;
    }
    
    for(int i=tpStart[source]; i<tpOrdered.size(); i++){
	int index = tpOrdered[i];
	if(vertexList[index].t > t_end || localDist[index] == infinity) //the modified version
	   continue;	
	int u = vertexList[index].u;
	distances[u] = min(distances[u], localDist[index]); //the modified version
	//first, take care of the next chain neighbor
	if(index+1 < voutStart[u+1] && vertexList[index+1].t <= t_end)
	   localDist[index+1] = min(localDist[index+1], localDist[index]);
	//now, take care of the other neighbors
	for(int j=0; j<vertexList[index].adjList.size(); j++){
	   int neigh = vertexList[index].adjList[j].first;
	   if(vertexList[neigh].t > t_end)
		continue;
	   localDist[neigh] = min(localDist[index]+1, localDist[neigh]);
	}	   
    }

    /*use localDist to compute distances*/
    //for(int i=0; i<vertexList.size()-1; i++) //ingore the dummy node
	//if(vertexList[i].t >= t_start && vertexList[i].t <= t_end){
	  // int u = vertexList[i].u; 
	   //distances[u] = min(distances[u], localDist[i]);
	//}

    t.stop();
    time_sum += t.GetRuntime();

    /*for debugging only*/
    for(int i=0; i<distances.size(); i++)
	cout << distances[i] << endl;
}
//--------------//

void Graph::print_avg_time()
{
	cout<<"Average time: " << time_sum/numSources <<endl;
}
