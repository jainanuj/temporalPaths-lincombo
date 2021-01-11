#ifndef GRAPH_H_HHWU
#define GRAPH_H_HHWU

#include <cstdio>
#include <vector>
#include <cstring>
#include <algorithm>
#include <set>
#include <queue>
#include <iostream>
#include "Timer.h"

using namespace std;

const int infinity = 1e9;

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


class Graph
{
public:
    Graph() {}
    Graph(const char* filePath); // input file
    void initial_query(const char* filePath); // query file
    void initial_query();
    void initial_ds_ea();
    void initial_ds_ld();
    void initial_ds_f();
    void initial_ds_s();
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
    void run_earliest_arrival(const char* filePath); // output the result
    void earliest_arrival(int source, FILE * file);  
    void run_latest_departure(const char* filePath); 
    void latest_departure(int source, FILE * file); 
    void run_fastest(const char* filePath);
    void fastest(int source, FILE * file);
    void run_shortest(const char* filePath);
    void shortest(int source, FILE * file);
    void print_result(const int source, const vector<int>& t_time, FILE * file);
    void print_result_ld(const int source, const vector<int>& t_time, FILE * file); 
    void print_avg_time(const char* filePath1, const char* filePath2);

public:
    vector< Edge > edge_list;
    vector< int > sources;
    int V, static_E, dynamic_E;
    int t_start, t_end;
    double time_sum;
	vector <int> arr_time, f_time;
	vector < set < pair< int, int > > > ft_timepair; // arrival time, starting time
	vector < set < pair< int, int > > > st_timepair; // arrival time, shortest distance 
};


Graph::Graph(const char* filePath)
{
    FILE* file = fopen(filePath,"r");
	int x;
   	x=fscanf(file, "%d %d",&V, &dynamic_E);

	arr_time.resize(V);
	f_time.resize(V);
	ft_timepair.resize(V);
	st_timepair.resize(V);
	
	Edge e; 
    for(int i = 0; i < dynamic_E; i ++)
    {
        x=fscanf(file, "%d %d %d %d",&e.u, &e.v, &e.t, &e.w);
        edge_list.push_back(e);
    }

    fclose(file);
}

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


void Graph::initial_ds_ea()
{
	for(int i=0; i<V; i++){
		arr_time[i]= infinity;
	}

}

void Graph::initial_ds_ld()
{
	for(int i=0; i<V; i++){
		arr_time[i]= -1;
	}

}

void Graph::initial_ds_f()
{	
	for(int i=0; i<V; i++){
		f_time[i] = infinity;
		ft_timepair[i].clear();
	}

}

void Graph::initial_ds_s()
{	
	for(int i=0; i<V; i++){
		f_time[i] = infinity;
		st_timepair[i].clear();
	}

}


void Graph::run_earliest_arrival()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	initial_ds_ea();
    	earliest_arrival(sources[i]);
    }
	
	print_avg_time();
}

void Graph::earliest_arrival(int source)
{
	Timer t;
	t.start();
		
	arr_time[source]=t_start;
	
	for(int i=0; i<dynamic_E; i ++) {
		if(edge_list[i].t<t_end){
			if (edge_list[i].t>=arr_time[edge_list[i].u] && edge_list[i].t+edge_list[i].w<arr_time[edge_list[i].v] && edge_list[i].t+edge_list[i].w<=t_end){
				arr_time[edge_list[i].v] = edge_list[i].t+edge_list[i].w;
			}
		}
		else {
			break;
		}	
	}
	
	
	t.stop();
	time_sum += t.GetRuntime();
}

void Graph::run_earliest_arrival(const char* filePath)
{
	time_sum=0;
	
	FILE* file = fopen(filePath,"w");
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	initial_ds_ea();
    	earliest_arrival(sources[i], file);
    }
    
    fclose(file);
    
    print_avg_time();

}


void Graph::earliest_arrival(int source, FILE * file)
{
	Timer t;
	t.start();
		
	arr_time[source]=t_start;
	
	for(int i=0; i<dynamic_E; i ++) {
		if(edge_list[i].t<t_end){
			if (edge_list[i].t>=arr_time[edge_list[i].u] && edge_list[i].t+edge_list[i].w<arr_time[edge_list[i].v] && edge_list[i].t+edge_list[i].w<=t_end){
				arr_time[edge_list[i].v] = edge_list[i].t+edge_list[i].w;
			}	
		}
		else {
			break;
		}
	} 
	
	t.stop();
	time_sum += t.GetRuntime();
	
	print_result(source, arr_time, file);
}

void Graph::print_result(const int source, const vector<int>& t_time, FILE * file)
{
	
	for(int i = 0 ;i < V ;i ++)
    { 
    	if(t_time[i]!=infinity){
    		fprintf(file, "%d %d %d %d %d\n", source, i, t_start, t_end, t_time[i]);
    	}
    }

}

void Graph::run_latest_departure()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	initial_ds_ld();
    	latest_departure(sources[i]);
    }
	
	print_avg_time();
}

void Graph::latest_departure(int source)
{
	Timer t;
	t.start();
	
	arr_time[source]=t_end;
	
	for(int i=dynamic_E-1; i>=0; i --) {
		if(edge_list[i].t>=t_start){
			if (edge_list[i].t+edge_list[i].w <= arr_time[edge_list[i].u] && edge_list[i].t > arr_time[edge_list[i].v]){
				arr_time[edge_list[i].v] = edge_list[i].t;
			}	
		}
		else {
			break;
		}
	}
	
	t.stop();
	time_sum += t.GetRuntime();
}


void Graph::run_latest_departure(const char* filePath)
{
	time_sum=0;
	
	FILE* file = fopen(filePath,"w");
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	initial_ds_ld();
    	latest_departure(sources[i], file);
    }
    
    fclose(file);
    
    print_avg_time();

}

void Graph::latest_departure(int source, FILE * file)
{
	Timer t;
	t.start();
		
	arr_time[source]=t_end;
	
	for(int i=dynamic_E-1; i>=0; i --) {
		if(edge_list[i].t>=t_start){
			if (edge_list[i].t+edge_list[i].w <= arr_time[edge_list[i].u] && edge_list[i].t > arr_time[edge_list[i].v]){
				arr_time[edge_list[i].v] = edge_list[i].t;
			}	
		}
		else {
			break;
		}
	}
	
	t.stop();
	time_sum += t.GetRuntime();
	
	print_result_ld(source, arr_time, file);
}

void Graph::print_result_ld(const int source, const vector<int>& t_time, FILE * file)
{
	
	for(int i = 0 ;i < V ;i ++)
    { 
    	if(t_time[i]!=-1){
    		fprintf(file, "%d %d %d %d %d\n", source, i, t_start, t_end, t_time[i]);
    	}
    }

}


void Graph::run_fastest(const char* filePath)
{
	time_sum=0;
	
	FILE* file = fopen(filePath,"w");
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	initial_ds_f();
    	fastest(sources[i], file);
    }
    
    fclose(file);
    
    print_avg_time();

}

void Graph::fastest(int source, FILE * file)
{
	Timer t;
	t.start();
		
	f_time[source]=0;	
	
	set < pair< int, int > >::iterator it_tp, it_tp_low, it_tp_up, it_tp_1, it_tp_2;
	
	for(int i=0; i<dynamic_E; i ++) {
		if(edge_list[i].t<t_end){
			if (edge_list[i].t+edge_list[i].w<=t_end && edge_list[i].t >=t_start){
				if (edge_list[i].u == source){
					ft_timepair[edge_list[i].u].insert(make_pair(edge_list[i].t, edge_list[i].t));
				}
				
				if(!ft_timepair[edge_list[i].u].empty()){ // the path from x to u is not empty
					it_tp=ft_timepair[edge_list[i].u].upper_bound(make_pair(edge_list[i].t, infinity));
				
					if(it_tp != ft_timepair[edge_list[i].u].begin()){ //exists some pair arrived at or before t
						it_tp --;
						
						int a_t=edge_list[i].t+edge_list[i].w;
						int s_t=it_tp->second;
						
						if(a_t-s_t < f_time[edge_list[i].v])
							f_time[edge_list[i].v] = a_t-s_t;
					
						if(!ft_timepair[edge_list[i].v].empty()){ // the path from x to v is not empty
							it_tp_1=ft_timepair[edge_list[i].v].lower_bound(make_pair(a_t, s_t));
					
							if(it_tp_1 == ft_timepair[edge_list[i].v].begin() ){ // a_t is the smallest arrival time
								if (it_tp_1->first>a_t){ // the arrival time of it_tp_1 is larger than a_t
									it_tp_low=it_tp_1;
									for(it_tp_up=it_tp_low; it_tp_up != ft_timepair[edge_list[i].v].end(); it_tp_up++){
										if(it_tp_up->second > s_t){
											break;
										}
									}
									
									ft_timepair[edge_list[i].v].erase(it_tp_low,it_tp_up); //remove useless pairs
									ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
								}
							}
							else if (it_tp_1 == ft_timepair[edge_list[i].v].end()) { //a_t is the largest arrival time
								it_tp_2 = it_tp_1;
								it_tp_2 --;
								if(it_tp_2->first == a_t) {
									ft_timepair[edge_list[i].v].erase(it_tp_2);
									ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
								}
								else if (it_tp_2->second < s_t){
									ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t)); 
								}
							}
							else {
								if(it_tp_1->first > a_t) { // it_tp_1->first > a_t
									it_tp_2=it_tp_1;
									it_tp_2--;
									
									if(it_tp_2->first == a_t) {
										it_tp_low=it_tp_1;
										for(it_tp_up=it_tp_low; it_tp_up != ft_timepair[edge_list[i].v].end(); it_tp_up++){
											if(it_tp_up->second > s_t){
												break;
											}
										}
										
										ft_timepair[edge_list[i].v].erase(it_tp_2, it_tp_up);
										ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
									}
									else if (it_tp_2->second < s_t){ // it_tp_2->first < a_t
										it_tp_low=it_tp_1;
										for(it_tp_up=it_tp_low; it_tp_up != ft_timepair[edge_list[i].v].end(); it_tp_up++){
											if(it_tp_up->second > s_t){
												break;
											}
										}
										
										ft_timepair[edge_list[i].v].erase(it_tp_low, it_tp_up);
										ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
									}
								}
							}
						}
						else {
							ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
						}
						
					}	
				}
				
				
			}	
		}
		else {
			break;
		}
	} 
	
	t.stop();
	time_sum += t.GetRuntime();
	
	print_result(source, f_time, file);
}



void Graph::run_fastest()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	initial_ds_f();
    	fastest(sources[i]);
    }
    
    print_avg_time();

}

void Graph::fastest(int source)
{
	Timer t;
	t.start();
		
	f_time[source]=0;	
	
	set < pair< int, int > >::iterator it_tp, it_tp_low, it_tp_up, it_tp_1, it_tp_2;
	
	for(int i=0; i<dynamic_E; i ++) {
		if(edge_list[i].t<t_end){
			if (edge_list[i].t+edge_list[i].w<=t_end && edge_list[i].t >=t_start){
				if (edge_list[i].u == source){
					ft_timepair[edge_list[i].u].insert(make_pair(edge_list[i].t, edge_list[i].t));
				}
				
				if(!ft_timepair[edge_list[i].u].empty()){ // the path from x to u is not empty
					it_tp=ft_timepair[edge_list[i].u].upper_bound(make_pair(edge_list[i].t, infinity));
				
					if(it_tp != ft_timepair[edge_list[i].u].begin()){ //exists some pair arrived at or before t
						it_tp --;
						
						int a_t=edge_list[i].t+edge_list[i].w;
						int s_t=it_tp->second;
						
						if(a_t-s_t < f_time[edge_list[i].v])
							f_time[edge_list[i].v] = a_t-s_t;
					
						if(!ft_timepair[edge_list[i].v].empty()){ // the path from x to v is not empty
							it_tp_1=ft_timepair[edge_list[i].v].lower_bound(make_pair(a_t, s_t));
					
							if(it_tp_1 == ft_timepair[edge_list[i].v].begin() ){ // a_t is the smallest arrival time
								if (it_tp_1->first>a_t){ // the arrival time of it_tp_1 is larger than a_t
									it_tp_low=it_tp_1;
									for(it_tp_up=it_tp_low; it_tp_up != ft_timepair[edge_list[i].v].end(); it_tp_up++){
										if(it_tp_up->second > s_t){
											break;
										}
									}
									
									ft_timepair[edge_list[i].v].erase(it_tp_low,it_tp_up); //remove useless pairs
									ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
								}
							}
							else if (it_tp_1 == ft_timepair[edge_list[i].v].end()) { //a_t is the largest arrival time
								it_tp_2 = it_tp_1;
								it_tp_2 --;
								if(it_tp_2->first == a_t) {
									ft_timepair[edge_list[i].v].erase(it_tp_2);
									ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
								}
								else if (it_tp_2->second < s_t){
									ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t)); 
								}
							}
							else {
								if(it_tp_1->first > a_t) { // it_tp_1->first > a_t
									it_tp_2=it_tp_1;
									it_tp_2--;
									
									if(it_tp_2->first == a_t) {
										it_tp_low=it_tp_1;
										for(it_tp_up=it_tp_low; it_tp_up != ft_timepair[edge_list[i].v].end(); it_tp_up++){
											if(it_tp_up->second > s_t){
												break;
											}
										}
										
										ft_timepair[edge_list[i].v].erase(it_tp_2, it_tp_up);
										ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
									}
									else if (it_tp_2->second < s_t){ // it_tp_2->first < a_t
										it_tp_low=it_tp_1;
										for(it_tp_up=it_tp_low; it_tp_up != ft_timepair[edge_list[i].v].end(); it_tp_up++){
											if(it_tp_up->second > s_t){
												break;
											}
										}
										
										ft_timepair[edge_list[i].v].erase(it_tp_low, it_tp_up);
										ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
									}
								}
							}
						}
						else {
							ft_timepair[edge_list[i].v].insert(make_pair(a_t, s_t));
						}
						
					}	
				}
				
				
			}	
		}
		else {
			break;
		}
	}
	
	t.stop();
	time_sum += t.GetRuntime();
}


void Graph::run_shortest(const char* filePath)
{
	time_sum=0;
	
	FILE* file = fopen(filePath,"w");
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	initial_ds_s();
    	shortest(sources[i], file);
    }
    
    fclose(file);
    
    print_avg_time();

}

void Graph::shortest(int source, FILE * file)
{
	Timer t;
	t.start();
	
	f_time[source]=0;	
	
	set < pair< int, int > >::iterator it_tp, it_tp_low, it_tp_up, it_tp_1, it_tp_2;
	
	for(int i=0; i<dynamic_E; i ++) {
		if(edge_list[i].t<t_end){
			if (edge_list[i].t+edge_list[i].w<=t_end && edge_list[i].t >=t_start){
				if (edge_list[i].u == source){
					st_timepair[edge_list[i].u].insert(make_pair(edge_list[i].t, 0));
				}
				
				if(!st_timepair[edge_list[i].u].empty()){ // the path from x to u is not empty
					it_tp=st_timepair[edge_list[i].u].upper_bound(make_pair(edge_list[i].t, infinity));
				
					if(it_tp != st_timepair[edge_list[i].u].begin()){ //exists some pair arrived at or before t
						it_tp --;
						
						int a_t=edge_list[i].t+edge_list[i].w;
						int s_d=it_tp->second+edge_list[i].w;
						
						if(s_d < f_time[edge_list[i].v])
							f_time[edge_list[i].v] = s_d;
					
						if(!st_timepair[edge_list[i].v].empty()){ // the path from x to v is not empty
							it_tp_1=st_timepair[edge_list[i].v].upper_bound(make_pair(a_t, s_d));
					
							if(it_tp_1 == st_timepair[edge_list[i].v].begin() ){ // a_t is the smallest arrival time
								it_tp_low=it_tp_1;
								for(it_tp_up=it_tp_low; it_tp_up != st_timepair[edge_list[i].v].end(); it_tp_up++){
									if(it_tp_up->second < s_d){
										break;
									}
								}
								
								st_timepair[edge_list[i].v].erase(it_tp_low,it_tp_up); //remove useless pairs
								st_timepair[edge_list[i].v].insert(make_pair(a_t, s_d));
								
							}
							else if (it_tp_1 == st_timepair[edge_list[i].v].end()) { //a_t is the largest arrival time
								it_tp_2 = it_tp_1;
								it_tp_2 --;
					
								if (it_tp_2->first < a_t && it_tp_2->second > s_d){
									st_timepair[edge_list[i].v].insert(make_pair(a_t, s_d)); 
								}
							}
							else {
								it_tp_2=it_tp_1;
								it_tp_2--;
								
								if(it_tp_2->first < a_t && it_tp_2->second > s_d) {
									it_tp_low=it_tp_1;
									for(it_tp_up=it_tp_low; it_tp_up != st_timepair[edge_list[i].v].end(); it_tp_up++){
										if(it_tp_up->second < s_d){
											break;
										}
									}
									
									st_timepair[edge_list[i].v].erase(it_tp_low, it_tp_up);
									st_timepair[edge_list[i].v].insert(make_pair(a_t, s_d));
								}
							}
						}
						else {
							st_timepair[edge_list[i].v].insert(make_pair(a_t, s_d));
						}
						
					}	
				}
				
				
			}	
		}
		else {
			break;
		}
	}
	

	t.stop();
	time_sum += t.GetRuntime();
	
	print_result(source, f_time, file);
}


void Graph::run_shortest()
{
	time_sum=0;
	
	for(int i = 0 ;i < sources.size() ;i ++)
    { 
    	initial_ds_s();
    	shortest(sources[i]);
    }
    
    print_avg_time();

}

void Graph::shortest(int source)
{
	Timer t;
	t.start();
		
	f_time[source]=0;	
	
	set < pair< int, int > >::iterator it_tp, it_tp_low, it_tp_up, it_tp_1, it_tp_2;
	
	for(int i=0; i<dynamic_E; i ++) {
		if(edge_list[i].t<t_end){
			if (edge_list[i].t+edge_list[i].w<=t_end && edge_list[i].t >=t_start){
				if (edge_list[i].u == source){
					st_timepair[edge_list[i].u].insert(make_pair(edge_list[i].t, 0));
				}
				
				if(!st_timepair[edge_list[i].u].empty()){ // the path from x to u is not empty
					it_tp=st_timepair[edge_list[i].u].upper_bound(make_pair(edge_list[i].t, infinity));
				
					if(it_tp != st_timepair[edge_list[i].u].begin()){ //exists some pair arrived at or before t
						it_tp --;
						
						int a_t=edge_list[i].t+edge_list[i].w;
						int s_d=it_tp->second+edge_list[i].w;
						
						if(s_d < f_time[edge_list[i].v])
							f_time[edge_list[i].v] = s_d;
					
						if(!st_timepair[edge_list[i].v].empty()){ // the path from x to v is not empty
							it_tp_1=st_timepair[edge_list[i].v].upper_bound(make_pair(a_t, s_d));
					
							if(it_tp_1 == st_timepair[edge_list[i].v].begin() ){ // a_t is the smallest arrival time
								it_tp_low=it_tp_1;
								for(it_tp_up=it_tp_low; it_tp_up != st_timepair[edge_list[i].v].end(); it_tp_up++){
									if(it_tp_up->second < s_d){
										break;
									}
								}
								
								st_timepair[edge_list[i].v].erase(it_tp_low,it_tp_up); //remove useless pairs
								st_timepair[edge_list[i].v].insert(make_pair(a_t, s_d));
								
							}
							else if (it_tp_1 == st_timepair[edge_list[i].v].end()) { //a_t is the largest arrival time
								it_tp_2 = it_tp_1;
								it_tp_2 --;
					
								if (it_tp_2->first < a_t && it_tp_2->second > s_d){
									st_timepair[edge_list[i].v].insert(make_pair(a_t, s_d)); 
								}
							}
							else {
								it_tp_2=it_tp_1;
								it_tp_2--;
								
								if(it_tp_2->first < a_t && it_tp_2->second > s_d) {
									it_tp_low=it_tp_1;
									for(it_tp_up=it_tp_low; it_tp_up != st_timepair[edge_list[i].v].end(); it_tp_up++){
										if(it_tp_up->second < s_d){
											break;
										}
									}
									
									st_timepair[edge_list[i].v].erase(it_tp_low, it_tp_up);
									st_timepair[edge_list[i].v].insert(make_pair(a_t, s_d));
								}
							}
						}
						else {
							st_timepair[edge_list[i].v].insert(make_pair(a_t, s_d));
						}
						
					}	
				}
				
				
			}	
		}
		else {
			break;
		}
	}
	
	t.stop();
	time_sum += t.GetRuntime();
}

void Graph::print_avg_time()
{
	cout<<"Average time: " << time_sum/100 <<endl;
}

void Graph::print_avg_time(const char* filePath1, const char* filePath2)
{
	FILE* file = fopen(filePath2,"a");
	
	fprintf(file, "%s\t%f\n", filePath1, time_sum/100);;
	fclose(file);
}

#endif

