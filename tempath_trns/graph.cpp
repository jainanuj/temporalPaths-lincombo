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
	Vin[e.v].insert(e.t+e.w);
	Vout[e.u].insert(e.t);
   }

   /*for debug --> verified*/
   /*for(int i=0; i<V; i++){
	cout << "node " << i << " :" << endl;
	cout << "Vin: " << endl;
	for(int tt : Vin[i])
	   cout << tt << ", "; 
	cout << endl;
	cout << "Vout: " << endl;
	for(int tt : Vout[i])
	   cout << tt << ", "; 
	cout << endl; 
   }*/
   /***********/
   
   set <int>::iterator it; 
   //two maps to map (u, t) in Vin/Vout to their corresponding IDs in the transformed graph
   map<pair<int, int>, int> inMap;
   map<pair<int, int>, int> outMap;
   int index = 0; 
   //Node tmpNode; 
   int t; 
   for(int i=0; i<V; i++){
	for(it=Vin[i].begin(); it!=Vin[i].end(); it++){
	  t = *it;
	  //tmpNode = new Node(i, t, true);
          Node tmpNode(i, t, true); 
	  node_list.push_back(tmpNode);
	  inMap[make_pair(i, t)] = index++;
	}
	for(it=Vout[i].begin(); it!=Vout[i].end(); it++){
	  t = *it;
	  //tmpNode = new Node(i, t, false);
	  Node tmpNode(i, t, false); 
	  node_list.push_back(tmpNode);
	  outMap[make_pair(i, t)] = index++;
	}
   }

   /*for debugging --> verified*/
   //check the validity of the node_list
   /*for(int i=0; i<node_list.size(); i++){
	cout << "new index: " << i << endl; 
	cout << "u: " << node_list[i].u << ", t: " << node_list[i].t << ", "; 
	if(node_list[i].isVin) cout << "Vin" << endl;
	else cout << "Vout" << endl; 
   }
   //now check the validity of the maps
   map<pair<int, int>, int>::iterator m_it; 
   cout << "inMap content:" << endl; 
   for(m_it=inMap.begin(); m_it!=inMap.end(); m_it++){ 
	cout << "key: (u: " << (*m_it).first.first << ", t: " << (*m_it).first.second << ") , value (newID): " << (*m_it).second << endl; 
   }
   cout << "outMap content:" << endl; 
   for(m_it=outMap.begin(); m_it!=outMap.end(); m_it++){
	cout << "key: (u: " << (*m_it).first.first << ", t: " << (*m_it).first.second << ") , value (newID): " << (*m_it).second << endl; 
   }*/
   /***************/

   adj_list.resize(index); 

   //edge creation step a:
   set <int>::reverse_iterator rit;
   set <int>::iterator tmp_it;
   unordered_set<int> matched; //t value of Vout[i] nodes that are already matched
   for(int i=0; i<V; i++){ 
        matched.clear();
	//iterate over Vin[i] in descending order of t (reverse)
	for(rit = Vin[i].rbegin(); rit != Vin[i].rend(); rit++){ 
	   int tIn = *rit; 
	   tmp_it = Vout[i].lower_bound(tIn); //the first item that doesn't go before (t, newID)
           if(tmp_it == Vout[i].end()) 
		continue;
	   int tOut = *tmp_it; 
	   if(matched.find(tOut) == matched.end()){ 
		matched.insert(tOut);
	   	adj_list[inMap[make_pair(i, tIn)]].push_back(make_pair(outMap[make_pair(i, tOut)], 0));
	   }
	}
   }

   /*for debugging*/
   //print_adjList();
   /***************/

   //edge creation step b:
   set<int>::iterator it2; 
   for(int i=0; i<V; i++){
      it = it2 = Vin[i].begin(); 
      if(it2!=Vin[i].end()) it2++;
      while(it2 != Vin[i].end()){  
	   adj_list[inMap[make_pair(i, *it)]].push_back(make_pair(inMap[make_pair(i, *it2)], 0));
	   it++;
	   it2++;
      }
      it = it2 = Vout[i].begin();
      if(it2!=Vout[i].end()) it2++;
      while(it2 != Vout[i].end()){ 
	   adj_list[outMap[make_pair(i, *it)]].push_back(make_pair(outMap[make_pair(i, *it2)], 0));
	   it++;
	   it2++;
      }
   }

   /*for debugging*/
   //print_adjList();
   /***************/

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
    	//initial_ds_ea();
    	earliest_arrival(sources[i]);
    }
	
	print_avg_time();
}

void Graph::earliest_arrival(int source)
{
	Timer t;
	t.start();
		
	//TBD
	
	t.stop();
	time_sum += t.GetRuntime();
}

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
