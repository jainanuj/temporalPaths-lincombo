---------------------------------------------------------------
---------------------------------------------------------------
Optimal Temporal Walk Computation in Temporal Graphs
---------------------------------------------------------------
---------------------------------------------------------------

This tool computes single source optimal walks in a given temporal
graph, as described in the paper

   Efficient Computation of Optimal Temporal Walks 
   under Waiting-Time Constraints, Anne-Sophie Himmel, 
   Matthias Bentert, André Nichterlein, and Rolf Niedermeier.

If you use this code in scientific research, please cite the above
article.

The current version of this tool can be obtained from

    http://fpt.akt.tu-berlin.de/temporal-walks/ .

The tool is distributed under the terms of the GNU General Public
License Version 3 (GPL, see COPYING).
---------------------------------------------------------------
---------------------------------------------------------------

To run it, type

    ./implementation/tempath -e temporal-graph -s source -a criteria

Herein, temporal-graph is a file containing a temporal graph (a
multigraph with integer edge labels) encoded in lines of the form

     u v t w 

describing an edge from vertex u to vertex v at time step t with weight w.
Vertex names as well as the time step t have to be integers. The first 
line has to be in form

     n M 0

describing the number n of vertices and the number M of time-arcs.
The algorithm computes an optimal walk from the source to all vertices in 
the temporal graph (if it exists) under the given optimality criteria.

For further option and detailed information, type
     ./implementation/tempath --help

---------------------------------------------------------------

To transform a temporal graph with transmission times on the time-arcs, type 

     java -jar test_environment/parsegraph.jar temporal-graph

Herein, temporal-graph is a file containing a temporal graph encoded in lines 
of the form

     u v t lambda

describing an edge from vertex u to vertex v at time step t with transmission 
time lambda. Vertex names as well as the time step t have to be 
integers. The first line has to be in form

     n M

describing the number n of vertices and the number M of time-arcs. 
It return the temporal-graph as file 
     edge_temporal-graph 

and an equivalent temporal graph with no transmission times on the time-arcs as file 
     edge_zerotrans_temporal-graph
---------------------------------------------------------------

To automatically download the test data of our experiments and apply the 
transformation, type  
      ./test_environment/script.sh -download

For further option, type
      ./test_environment/script.sh -- help
---------------------------------------------------------------

-- Anne-Sophie Himmel
   24th August 2019




