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
   vector<set<int>> Tin; //the set of distinct in times for each node
   Tin.resize(V);
   for(Edge e : edge_list){
	Tin[e.v].insert(e.t+e.w);
   }

   set <int>::iterator it; 
   //to map (u, t) to their corresponding IDs in the transformed graph
   map<pair<int, int>, int> inMap;
   int index = 0; 
   int t; 
   for(int i=0; i<V; i++){
	if(Tin[i].size() == 0){
          Node tmpNode(i, 0); 
	  node_list.push_back(tmpNode);
	  Vin[i].insert(index);
	  inMap[make_pair(i, 0)] = index++;
	  Tin[i].insert(0);
	  continue;
	}
	for(it=Tin[i].begin(); it!=Tin[i].end(); it++){
	  t = *it;
          Node tmpNode(i, t); 
	  node_list.push_back(tmpNode);
	  Vin[i].insert(index);
	  inMap[make_pair(i, t)] = index++;
	}
   }

   /*cout << "node_list:" << endl;
   for(int i=0; i<node_list.size(); i++)
	cout << "u: " << node_list[i].u << ", t: " << node_list[i].t << endl;*/

   //filling in the outVec vector:
   for(Edge e : edge_list){
	int neighID = inMap[make_pair(e.v, e.t+e.w)];
	outVec[e.u].push_back(make_pair(neighID, e.t));
   }

   adj_list.resize(index); 

   //edge creation step:
   int edge_cnt = 0; //just for debugging
   for(Edge e : edge_list){
	int v_index = inMap[make_pair(e.v, e.t+e.w)];
	for(auto set_it = Tin[e.u].begin(); set_it != Tin[e.u].end(); set_it++){
	   t = *set_it;
	   if(t <= e.t){
	      int u_index = inMap[make_pair(e.u, t)];
	      adj_list[u_index].push_back(make_pair(v_index, e.w));
	      edge_cnt++; // just for debugging
	   }
	}
   }

   cout << "number of edges after transform: " << edge_cnt << endl;
   cout << "number of nodes after transform: " << index << endl;

   //filling up the reverse adjacency list:
   rev_adjList.resize(index);
   for(int i=0; i<index; i++){
	for(auto neigh=adj_list[i].begin(); neigh!=adj_list[i].end(); neigh++){
	    rev_adjList[neigh->first].push_back(make_pair(i, neigh->second)); 
        }
   }

   //for debugging:
   //print_adjList();


   //for debugging
   /*cout << "source: 14865" << endl;
   int from_id = inMap[make_pair(14865, 0)];
   int to_id = inMap[make_pair(14517, 549505)];
   cout << "from_id: " << from_id << endl;
   cout << "adj_list[from_id].size(): " << adj_list[from_id].size() << endl;
   for(auto it2 = adj_list[from_id].begin(); it2 != adj_list[from_id].end(); it2++)
	if(it2->first == to_id){
	    cout << "the link is found" << endl;
	    return;
        }
   cout << "the link is not found!!!!!" << endl;*/
}

void Graph::print_adjList(){  
   cout << "(u1, t1) (u2, t2) W" << endl; 

   for(int i=0; i<adj_list.size(); i++){

     for(int j=0; j<adj_list[i].size(); j++){	

        //print out the source node 
	cout << "(" << node_list[i].u << ", " << node_list[i].t << ") "; 

	//print out the destination node
	int id = adj_list[i][j].first;
	int w = adj_list[i][j].second; 
        cout << "(" << node_list[id].u << ", " << node_list[id].t << ") ";
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
        //cout << "node " << node_list[*it].u << " pushed into the queu. Node id: " << *it << "node time: " << node_list[*it].t << endl; 
        Q.push(*it);
    }

    while(!Q.empty()){
	int node = Q.front(); 
        //cout << "node " << node_list[node].u << " extracted from Q, node_id(node): " << node << endl;
	Q.pop();
	//cout << "adj_list[node].size(): " << adj_list[node].size() << endl;
	for(auto neighbor=adj_list[node].begin(); neighbor!=adj_list[node].end(); neighbor++){
	    /*TO BE OPTIMIZED*/
	    /*the time we enter the "source node" doesn't matter, but the time we exit it does*/
	    int exitTime = node_list[neighbor->first].t - neighbor->second;
	    //cout << "exit time: " << exitTime << endl;
	    if(exitTime < t_start || exitTime > t_end)
		continue;
	    /*UP TO HERE*/
	    int nID = neighbor->first; 
	    Node neiNode = node_list[nID];
	    //cout << "neighbor: " << neiNode.u << endl;
	    if(!visited[nID] && neiNode.t >= t_start && neiNode.t <= t_end){
		visited[nID] = true; 
		//cout << "neighbor pushed into Q" << endl;
		Q.push(nID);
		if(neiNode.t < distances[neiNode.u]){
		   distances[neiNode.u] = neiNode.t;
		  // cout << "neighbor distance updated" << endl; 
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

        /*cout << "startPoint:" << endl;	
	cout << "node: " << it->first << ", t: " << it->second << endl;
	cout << "ts: " << ts << endl;*/

	//here it->first is a neighbor of the source
        visited[it->first] = true; 
        Q.push(it->first);
	int tmp_time =  node_list[it->first].t-ts;
	int tmp_u = node_list[it->first].u;
	if(tmp_time < distances[tmp_u]){
	   distances[tmp_u] = tmp_time;
	   //cout << "distance updated to " << tmp_time << endl;
	}

	while(!Q.empty()){
	    /*cout << "at the beginning of the while loop" << endl;
	    cout << "visited: " << endl;
	    for(int ii=0; ii<visited.size(); ii++)	
		if(visited[ii])
			cout << "true, ";
		else
			cout << "false, ";
	    cout << endl;
	    cout << "distances:" << endl;
	    for(int ii=0; ii<distances.size(); ii++)
		cout << distances[ii] << ", ";
	    cout << endl;*/
	    int node = Q.front(); 
	    Q.pop();
	    //cout << "node " << node_list[node].u << "popped" << endl;
	    for(auto neighbor=adj_list[node].begin(); neighbor!=adj_list[node].end(); neighbor++){
		int nID = neighbor->first; 
		Node neiNode = node_list[nID];
	        /*cout << "neighbor: " << endl;
		cout << "u: " << neiNode.u << ", t: " << neiNode.t << endl;*/
		if(!visited[nID] && neiNode.t >= t_start && neiNode.t <= t_end){
		   visited[nID] = true; 
		   Q.push(nID);
		   if((neiNode.t-ts) < distances[neiNode.u]){
		      distances[neiNode.u] = (neiNode.t-ts); 
		      //cout << "neighbor distance updated to " << neiNode.t-ts << endl;
		   }
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
