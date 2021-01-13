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
   for(Edge e : edge_list){
	//initially set all the newIDs (indices in the transformed graph) to -1
	Vin[e.v].insert(make_pair(e.t+e.w, -1));
	Vout[e.u].insert(make_pair(e.t, -1));
   }
   
   set <pair<int, int>>::iterator it; 
   //two maps to map (u, t) in Vin/Vout to their corresponding IDs in the transformed graph
   map<pair<int, int>, int> inMap;
   map<pair<int, int>, int> outMap;
   int index = 0; 
   //Node tmpNode; 
   int t; 
   for(int i=0; i<V; i++){
	for(it=Vin[i].begin(); it!=Vin[i].end(); it++){
	  t = it->first;
	  //tmpNode = new Node(i, t, true);
          Node tmpNode(i, t, true); 
	  node_list.push_back(tmpNode);
	  it->second = index; 
	  inMap[make_pair(i, t)] = index++;
	}
	for(it=Vout[i].begin(); it!=Vout[i].end(); it++){
	  t = it->first;
	  //tmpNode = new Node(i, t, false);
	  Node tmpNode(i, t, false); 
	  node_list.push_back(tmpNode);
	  it->second = index; 
	  outMap[make_pair(i, t)] = index++;
	}
   }

   adj_list.resize(index); 

   //edge creation step a:
   set <pair<int, int>>::reverse_iterator rit;
   set <pair<int, int>>::iterator tmp_it;
   unordered_set<int> matched; //t value of Vout[i] nodes that are already matched
   for(int i=0; i<V; i++){ 
        matched.clear();
	//iterate over Vin[i] in descending order of t (reverse)
	for(rit = Vin[i].rbegin(); rit != Vin[i].rend(); rit++){ 
	   int tIn = rit->first; 
	   tmp_it = Vout[i].lower_bound(*rit); //the first item that doesn't go before (t, newID) (*rit is a pair)
           if(tmp_it == Vout[i].end()) 
		continue;
	   int tOut = tmp_it->first; 
	   if(matched.find(tOut) == matched.end()){ 
		matched.insert(tOut);
	   	adj_list[rit->second].push_back(make_pair(tmp_it->second, 0));
	   }
	}
   }

   //edge creation step b:
   set<pair<int, int>>::iterator it2; 
   for(int i=0; i<V; i++){
      it = it2 = Vin[i].begin(); 
      if(it2!=Vin[i].end()) it2++;
      while(it2 != Vin[i].end()){  
	   adj_list[it->second].push_back(make_pair(it2->second, 0));
	   it++;
	   it2++;
      }
      it = it2 = Vout[i].begin();
      if(it2!=Vout[i].end()) it2++;
      while(it2 != Vout[i].end()){ 
	   adj_list[it->second].push_back(make_pair(it2->second, 0));
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
    set<pair<int, int>>::iterator it; 
    for(it = Vout[source].begin(); it != Vout[source].end(); it++){
	if(it->first >= t_start && it->first <= t_end){
	   visited[it->second] = true; //it->first: t, it->second: newID
           Q.push(it->second);
        }
    }

    while(!Q.empty()){
	int node = Q.top(); 
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
