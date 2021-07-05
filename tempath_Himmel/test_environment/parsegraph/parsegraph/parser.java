package parsegraph;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.PriorityQueue;
import java.util.concurrent.ThreadLocalRandom;

public class parser {
	
	public static int mode = 0;
	public static int start = 0;
	public static int end = 0;
	
	//use "java -jar parsegraph.jar number" for mode 0
	//use "java -jar parsegraph.jar number,number" for mode 1
	/**
	 * Mode 0: every node has the same dwell time \beta
	 * Mode 1: every node has a dwell time in [start,end]
	 * @param args
	 * @throws IOException
	 */
	public static void main(String[] args) throws IOException {
		
		if(args.length>0){
			parse(args);
		}
	}
	
	public static void parse(String[] args) throws IOException {
			String filename = args[0];
			String[] tmp = filename.split("/");
			String filename2 = tmp[tmp.length-1];
			int errors = 0;
			/*if(args.length>1){
				String flag = args[1];
				if(flag.matches("[0-9]*")){
					mode = 0;
					start = Integer.parseInt(flag);
					//option 1
				}else if(flag.matches("[0-9]*,[0-9]*")){
					//option 2
					start = Integer.parseInt(flag.split(",")[0]);
					end = Integer.parseInt(flag.split(",")[1]);
					mode = 0;
				}
			}*/
			
			
			int num_nodes = 0;
			int num_edges = 0;
			int n2 = 0;
			int e2= 0;
			HashMap<Integer, Integer> hash = new HashMap<Integer,Integer>();
			
			BufferedReader br = new BufferedReader(new FileReader(filename));
			String line;
			PriorityQueue<Edge> edges = new PriorityQueue<Edge>();
			PriorityQueue<Edge> edges2 = new PriorityQueue<Edge>();

			//added by sanaz:
			line = br.readLine(); //throw away the first line
			//System.out.println(line);			

			while((line=br.readLine())!=null){
				//System.out.println(line);	
				//line = line.split("/")[0].trim();
				//line = line.replaceAll("\\s+",",");				
				//if(line.matches("[0-9]*,[0-9]*,[-]*[0-9]*,[-]*[0-9]*")){
					String[] tmp2 = line.split(" ");

					//added by sanaz, to remove an exception made by the last line being only ""
					if(tmp2.length < 4)
					   break;

					int u = Integer.parseInt(tmp2[0]);		//start- x
					int v = Integer.parseInt(tmp2[1]);		//end- y
					int t = Integer.parseInt(tmp2[2]);		//weight- z - used to be w
					int w = Integer.parseInt(tmp2[3]);		//timestamp - w - used to be t
					
					/*if(z<1){	// if negative weight
						z=1;
					}*/
					
					if(!hash.containsKey(u)){
						hash.put(u, num_nodes);
						num_nodes++;
					}				
					if(!hash.containsKey(v)){
						hash.put(v, num_nodes);
						num_nodes++;
					}
					
					if(w!=0){
						e2++;
						n2++;
					}
					
					Edge e = new Edge(hash.get(u),hash.get(v),t,w);
					edges.add(e);
					num_edges++;

				/*}else{
					if(errors==10){
						System.out.println(line);
					}
					errors++;
				}*/
			}
			br.close();
			System.out.println("Num_errors: " + errors);
			
			if(num_nodes==0){
				return;
			}
			
			int curr = num_nodes;
			PrintWriter writer = new PrintWriter("edge_" + filename2, "UTF-8");
			writer.println(num_nodes + " " + num_edges);
			while(!edges.isEmpty()){
				Edge e = edges.poll();
				writer.println(e.toString());
				if(e.weigth!=0){
					Edge e_1 = new Edge(e.start,curr,e.time,0);
					Edge e_2;
					if((long)e.time+(long)e.weigth>2000000000){
						e_2 = new Edge(curr,e.end,2000000000,0);
					}else{
						e_2 = new Edge(curr,e.end,e.time+e.weigth,0);
					}
					curr++;
					e_1.weigth = e.weigth;
					edges2.add(e_1);
					edges2.add(e_2);
				}			
			}
			writer.close();
			
			PrintWriter writer2 = new PrintWriter("edge_zerotrans_" + filename2, "UTF-8");
			writer2.println((num_nodes+n2) + " " + (num_edges+e2) + " " + (num_nodes));
			while(!edges2.isEmpty()){
				writer2.println(edges2.poll().toString());
			}
			writer2.close();
	}
}
