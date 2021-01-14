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
    Vout.resize(V);
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
   vector<set<int>> Tout; //the set of distinct out times for each node
   Tin.resize(V);
   Tout.resize(V); 
   for(Edge e : edge_list){
	//initially set all the newIDs (indices in the transformed graph) to -1
	Tin[e.v].insert(e.t+e.w);
	Tout[e.u].insert(e.t);
   }

   set <int>::iterator it; 
   //two maps to map (u, t) in Tin/Tout to their corresponding IDs in the transformed graph
   map<pair<int, int>, int> inMap;
   map<pair<int, int>, int> outMap;
   int index = 0; 
   //Node tmpNode; 
   int t; 
   for(int i=0; i<V; i++){
	for(it=Tin[i].begin(); it!=Tin[i].end(); it++){
	  t = *it;
	  //tmpNode = new Node(i, t, true);
          Node tmpNode(i, t, true); 
	  node_list.push_back(tmpNode);
	  Vin[i].insert(index);
	  inMap[make_pair(i, t)] = index++;
	}
	for(it=Tout[i].begin(); it!=Tout[i].end(); it++){
	  t = *it;
	  //tmpNode = new Node(i, t, false);
	  Node tmpNode(i, t, false); 
	  node_list.push_back(tmpNode);
	  Vout[i].insert(index);
	  outMap[make_pair(i, t)] = index++;
	}
   }
   adj_list.resize(index); 

   /*for debugging*/
   /*for(int i=0; i<V; i++){
      cout << "for node " << i << ":" << endl;
      cout << "vin:" << endl;
      for(int idx : Vin[i])
	cout << "(" << node_list[idx].u << ", " << node_list[idx].t << ")" << endl;
      cout << "vout:" << endl;
      for(int idx : Vout[i])
	cout << "(" << node_list[idx].u << ", " << node_list[idx].t << ")" << endl;
   }*/
   /***************/

   //edge creation step a:
   set <int>::reverse_iterator rit;
   set <int>::iterator tmp_it;
   unordered_set<int> matched; //t value of Tout[i] nodes that are already matched
   for(int i=0; i<V; i++){ 
        matched.clear();
	//iterate over Tin[i] in descending order of t (reverse)
	for(rit = Tin[i].rbegin(); rit != Tin[i].rend(); rit++){ 
	   int tIn = *rit; 
	   tmp_it = Tout[i].lower_bound(tIn); //the first item that doesn't go before (t, newID)
           if(tmp_it == Tout[i].end()) 
		continue;
	   int tOut = *tmp_it; 
	   if(matched.find(tOut) == matched.end()){ 
		matched.insert(tOut);
	   	adj_list[inMap[make_pair(i, tIn)]].push_back(make_pair(outMap[make_pair(i, tOut)], 0));
	   }
	}
   }

   //edge creation step b:
   set<int>::iterator it2; 
   for(int i=0; i<V; i++){
      it = it2 = Tin[i].begin(); 
      if(it2!=Tin[i].end()) it2++;
      while(it2 != Tin[i].end()){  
	   adj_list[inMap[make_pair(i, *it)]].push_back(make_pair(inMap[make_pair(i, *it2)], 0));
	   it++;
	   it2++;
      }
      it = it2 = Tout[i].begin();
      if(it2!=Tout[i].end()) it2++;
      while(it2 != Tout[i].end()){ 
	   adj_list[outMap[make_pair(i, *it)]].push_back(make_pair(outMap[make_pair(i, *it2)], 0));
	   it++;
	   it2++;
      }
   }

   //edge creation step c: 
   for(Edge e : edge_list){
	adj_list[outMap[make_pair(e.u, e.t)]].push_back(make_pair(inMap[make_pair(e.v, e.t+e.w)], e.w));
   }
}
void Graph::print_adjList(){  
   cout << "(u1, t1, Vin1/Vout1) (u2, t2, Vin2/Vout2) W" << endl; 

   for(int i=0; i<adj_list.size(); i++){

     for(int j=0; j<adj_list[i].size(); j++){	

        //print out the source node 
	cout << "(" << node_list[i].u << ", " << node_list[i].t << ", "; 
	if(node_list[i].isVin)
	   cout << "Vin) ";
	else 
	   cout << "Vout) ";

	//print out the destination node
	int id = adj_list[i][j].first;
	int w = adj_list[i][j].second; 
        cout << "(" << node_list[id].u << ", " << node_list[id].t << ", ";
	if(node_list[id].isVin)
	   cout << "Vin) ";
	else 
	   cout << "Vout) ";
	cout << w << endl; 
     }
   }
}
//--------------

void Graph::initial_query()
{
	t_start = 0;
	t_end = infinity;
	
	int s;
	for(int i = 0 ;i < 100 ;i ++)
    {
    	s=rand()%V;
        sources.push_back(s);
    }

}


void Graph::initial_query(const char* filePath)
{
	t_start = 0;
	t_end = infinity;
	
	FILE* file = fopen(filePath,"r");
	int s, x;
	for(int i = 0 ;i < 100 ;i ++)
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
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//modified by sanaz:
	for(int j=0; j<V; j++)
	  distances[j] = infinity; 
	distances[sources[i]] = 0; 
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
    vector<int> distances(V, infinity);
    distances[source] = 0; 
    queue<int> Q; 

    /*initializing Q*/
    set<int>::iterator it; 
    for(it = Vout[source].begin(); it != Vout[source].end(); it++){
	if(node_list[*it].t >= t_start && node_list[*it].t <= t_end){
	   visited[*it] = true; //it->first: t, it->second: newID
           Q.push(*it);
        }
    }

    while(!Q.empty()){
	int node = Q.front(); 
	Q.pop();
	for(auto neighbor=adj_list[node].begin(); neighbor!=adj_list[node].end(); neighbor++){
	    int nID = neighbor->first; 
	    Node neiNode = node_list[nID];
	    if(!visited[nID] && neiNode.t >= t_start && neiNode.t <= t_end){
		visited[nID] = true; 
		Q.push(nID);
		if(neiNode.isVin == true && neiNode.t < distances[neiNode.u])
		   distances[neiNode.u] = neiNode.t; 
	    }
	}
    }

    /*for debugging*/
    /*cout << "distances:" << endl;
    for(int i=0; i<V; i++)
	cout << distances[i] << ", "; 
    cout << endl; */
    /***************/
	
    t.stop();
    time_sum += t.GetRuntime();
}
//-----------------

void Graph::run_latest_departure()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//initial_ds_ld();
    	latest_departure(sources[i]);
    }
	
	print_avg_time();
}

void Graph::latest_departure(int source)
{
	Timer t;
	t.start();
	
	//TBD
	
	t.stop();
	time_sum += t.GetRuntime();
}

void Graph::run_fastest()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//initial_ds_f();
    	fastest(sources[i]);
    }
    
    print_avg_time();

}

void Graph::fastest(int source)
{
	Timer t;
	t.start();
		
	//TBD
	
	t.stop();
	time_sum += t.GetRuntime();
}

void Graph::run_shortest()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	//initial_ds_s();
    	shortest(sources[i]);
    }
    
    print_avg_time();

}

void Graph::shortest(int source)
{
	Timer t;
	t.start();

	//TBD		
	
	t.stop();
	time_sum += t.GetRuntime();
}

void Graph::print_avg_time()
{
	cout<<"Average time: " << time_sum/100 <<endl;
}
