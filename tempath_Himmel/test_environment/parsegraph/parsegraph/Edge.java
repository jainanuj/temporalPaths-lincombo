package parsegraph;


public class Edge implements Comparable{
	int start;
	int end;
	int time;
	int weigth;

	public Edge(int start, int end, int time, int weight) {
		this.start= start;
		this.end = end;
		this.time = time;
		this.weigth = weight;
	}
	
	@Override
	public String toString() {
		return this.start + " " + this.end + " " + this.time + " " + this.weigth;
	}

	@Override
	public int compareTo(Object o) {
		Edge tmp = (Edge) o;
		if(this.time> tmp.time){
			return 1;
		}else if(this.time==tmp.time){
			return 0;
		}
		return -1;
	}
}
