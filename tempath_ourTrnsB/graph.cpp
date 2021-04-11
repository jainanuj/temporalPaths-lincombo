#include "graph.h"

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
    int i = listSize-1;
    while(i>=0){
        int minTW = edge_list[i].t+edge_list[i].w;
        int curU = edge_list[i].u;
        int curV = edge_list[i].v;
        while(--i>=0 && edge_list[i].u == curU && edge_list[i].v == curV ){
             int tmpTW = edge_list[i].t + edge_list[i].w;
             if(tmpTW >= minTW)
                edge_list.erase(edge_list.begin()+i);
             else
                minTW = tmpTW;
        }
    }

    /*for debugging*/
    /*cout << "the edge_list after removing the dominant edges:" << endl;
 *     for(int i=0; i<edge_list.size(); i++)
 *             cout << "u: " << edge_list[i].u << ", v: " << edge_list[i].v << ", t: " << edge_list[i].t << ", w: " << edge_list[i].w << endl;*/
    return;
}

//added by sanaz
void Graph::transform(){
   vector<set<int>> Tout; //the set of distinct out times for each node
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
	Tout[i].insert(infinity); //dumb node, so that the nodes with an empty Tout but non-empty Tin are accounted for
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
	      edge_cnt++; // just for debugging
	      break;
	   }
	}
   }


   //for debugging only:
   /*for(int i = voutStart[6337]; i< voutStart[6338]; i++)
   for(auto ii=vertexList[i].adjList.begin(); ii!=vertexList[i].adjList.end(); ii++)
	if(vertexList[ii->first].u == 3713)
		cerr << "there is a link from node 6337 to node 3713" << endl;
   cerr << "there is noooo link from node 6337 to node 3713" << endl;
   //for debugging only:
   for(int i=voutStart[5751]; i<voutStart[5752]; i++)
   for(auto ii=vertexList[i].adjList.begin(); ii!=vertexList[i].adjList.end(); ii++)
        if(vertexList[ii->first].u == 5062)
                cerr << "there is a link from node 5751 to node 5062" << endl;
   cerr << "there is noooo link from node 5751 to node 5062" << endl;*/

   //DFS, for debugging only:
   int source = 6347;
   int dest = 5530;
   for(int i=voutStart[source]; i<voutStart[source+1]; i++){
	vector<bool> visited(vertexList.size(), false);
	vector<int> stack;
	stack.insert(stack.begin(), i);
	DFS(i, dest, visited, stack);
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

void Graph::DFS(int sourceNewID, int destOldID, vector<bool> visited, vector<int> stack){
     //cerr << "inside the DFS function" << endl;
     if(vertexList[sourceNewID].u == destOldID){
	int i=stack.size()-1; 
	for(; i>0; i--)
	    cerr << vertexList[stack[i]].u << " -> ";
	cerr << vertexList[stack[i]].u << endl;
	return;
     }
     for(auto it=vertexList[sourceNewID].adjList.begin(); it != vertexList[sourceNewID].adjList.end(); it++){
	int neigh = it->first;
	if(!visited[neigh]){
	   visited[neigh] = true;
	   stack.insert(stack.begin(), neigh);
	   DFS(neigh, destOldID, visited, stack);
	   stack.erase(stack.begin());
	   visited[neigh] = false;
	}
     }
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
	int ts = vertexList[it].t; //start time
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
		int inTime = vertexList[node].t + neighbor->second;
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
    //for(int i=0; i<distances.size(); i++)
	//cout << distances[i] << endl;
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
	   //TO BE OPTIMIZED
	   //Vin[source] need not be in range, but the exit times should be
	   int exitTime = vertexList[neigh->first].t - neigh->second;
	   if(exitTime < t_start || exitTime > t_end)
		continue;
	   //UP TO HERE
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

//added by sanaz//
void Graph::run_minhop()
{
    time_sum=0;
	
    for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//modified by sanaz:
	for(int j=0; j<V; j++)
	  distances[j] = infinity; 
	distances[sources[i]] = 0; 
    	minhop(sources[i]);
    }
    
    print_avg_time();

}

void Graph::minhop(int source)
{
    Timer t;
    t.start();

    //for debugging only://
    vector<Node> parents;
    parents.resize(V);
    //up to here---------//

    vector<bool> newLive(V, false);
    vector<int> eout(V, infinity);
    vector<bool> visited(vertexList.size(), false);
    queue<int> Q;

    Q.push(source);
    eout[source] = t_start;
    int hopCount = 1;
    int count = 1; 

    while(Q.size() > 0){
	int qSize = Q.size();
	for(int i=0; i<qSize; i++){
    	    int u = Q.front();
	    Q.pop();
	    //try all feasible touts from u not tried before
	    for(int uu = voutStart[u]; uu < voutStart[u+1] && vertexList[uu].t <= t_end && !visited[uu]; uu++){
		if(vertexList[uu].t < eout[u])
		   continue;
		visited[uu] = true;
		for(auto it=vertexList[uu].adjList.begin(); it != vertexList[uu].adjList.end(); it++){
		    int neigh = it->first; //new index
		    int tOut = vertexList[neigh].t;
		    int vv = vertexList[neigh].u; //old index
		    /*a big assumption: hop count is always < infinity*/
		    //the first condition added because maybe the target node has no out-time other than infinity used for the dumb vout node
		    if(distances[vv] == infinity || tOut < eout[vv]){ 
			if(eout[vv] == infinity){ //vv is newly reached
			   distances[vv] = hopCount;
			   parents[vv] = vertexList[uu]; //for debugging only
			   if(++count == V){ //all vertices reached
				t.stop();
				time_sum += t.GetRuntime();
				//for debugging only
				//for(int i=0; i<distances.size(); i++)
				     //cout << "target_node: " << i << " - " << distances[i] << endl;
				for(int i=0; i<V; i++)
        				cout << "node: " << i << ", parent.u: " << parents[i].u << ", parent.t: " << parents[i].t << endl;
				return;
			   }
			}//done
			eout[vv] = tOut;
			if(!newLive[vv]){
			   newLive[vv] = true;
			   Q.push(vv);
			}
		    }
		}
	    }
	}
        vector<bool> tmpLive(V, false);
	swap(tmpLive, newLive);
	hopCount++;
    }        

    t.stop();
    time_sum += t.GetRuntime();

    //for debugging only
    //for(int i=0; i<distances.size(); i++)
       // cout << "target_node: " << i  << " - " << distances[i] << endl;

    for(int i=0; i<V; i++)
        cout << "node: " << i << ", parent.u: " << parents[i].u << ", parent.t: " << parents[i].t << endl;
}

//--------------//

void Graph::print_avg_time()
{
	cout<<"Average time: " << time_sum/numSources <<endl;
}
