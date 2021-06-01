#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "spiderprimitives.h"



vector<edge> connect_point_with_edge(int pointindex, int edgeindex, vector<edge> edgelist) {
	edgelist.push_back(edge{ pointindex,edgelist[edgeindex].endpts[1] });
	edgelist[edgeindex].endpts[1] = pointindex;
	return edgelist;
}

vector<pt> animate(vector<pt> vertexlist, edgeList edgelist) {
	int counter = 0;
	float k = 1.1f;
	float eps = 0.1f;
	int index1,index2;


	for (const edge& edge : edgelist.edgeIndices) {
		index1 = edge.endpts[0];
		index2 = edge.endpts[1];
		//vertexlist[index1].isfixed = false;
		//vertexlist[index2].isfixed = false;

		if (!vertexlist[index1].isfixed) {
			vertexlist[index1].movevec += 1*k * (edge.currentlength - edge.restlength) *
				normalize(vertexlist[index2].pos - vertexlist[index1].pos);
		}
		if (!vertexlist[index2].isfixed) {
			vertexlist[index2].movevec +=1* k * (edge.currentlength - edge.restlength) *
				normalize(vertexlist[index1].pos - vertexlist[index2].pos);
		}

		//copyverts[counter]=point+
		counter++;
	}
	
	for (int i = 0; i < (int)vertexlist.size(); i++)
	{
		vertexlist[i].pos += eps*vertexlist[i].movevec;
		vertexlist[i].movevec = vec2{0,0};
	}


	return vertexlist;
};

vec2 linecp(vec2 start, vec2 end, vec2 point) {
	vec2 ab = end - start;
	vec2 ap = point - start;
	float t = dot(ab, ap) / (length(ab) * length(ab));
	if (t > 1.0f)
	{
		return end;
	}
	else if (t < 0.0f)
	{
		return start;
	}
	else	return  start + t * ab;
}

pair<int, vec2> closestedge(vec2 mousepos, vector<edge> edgelist, vector<pt> veclist) {
	float curdist = 100000000000000.0f;
	vec2 closestp;
	int counter = 0;
	int referencer;
	for (const edge& arr : edgelist)
	{
		vec2 distp = linecp(veclist[arr.endpts[0]].pos, veclist[arr.endpts[1]].pos, mousepos);
		float dist = distance(distp, mousepos);
		if (dist < curdist)
		{
			curdist = dist;
			closestp = distp;
			referencer = counter;
		}
		counter++;
	}

	return make_pair(referencer, closestp);

}

int nClosestEdge (vec2 *mousepos, vector<edge> edgelist, vector<pt> veclist) {
	float curdist = 100000000000000.0f;
	vec2 closestp;
	int counter = 0;
	int referencer;
	for (const edge& arr : edgelist)
	{
		vec2 distp = linecp(veclist[arr.endpts[0]].pos, veclist[arr.endpts[1]].pos, *mousepos);
		float dist = distance(distp, *mousepos);
		if (dist < curdist)
		{
			curdist = dist;
			closestp = distp;
			referencer = counter;
		}
		counter++;
		
	}
	*mousepos = closestp;
	return referencer;

}



string list_edges(vector<edge> edgelist) {
	ostringstream oss;

	oss << "list of edges: ";
	oss << "{";
	for (const edge& arr : edgelist) {

		oss << "[" << arr.endpts[0] << "," << arr.endpts[1] << "]" << "," << arr.currentlength-arr.restlength << "   ";
		/*outstring += (arr[0]);
		outstring += ",";
		outstring += arr[1];
		outstring += "]";*/


	}
	oss << "}" << endl;
	//outstring += "\n";
	return oss.str();
}

string list_vertices(vector<pt> vertexlist) {
	ostringstream oss;
	oss << "list of vertices: ";
	for (const pt& arr : vertexlist) {

		oss << "[" << arr.pos[0] << "," << arr.pos[1] << "]" << "  ";
	}
	oss << endl;

	return oss.str();
}


void edgeList::nconnect_point_with_edge(int pointindex, int edgeindex,vec2 cachedpos) {
	vec2 pointpos = pointlist->at(pointindex).pos;

	float restlength1 = distance(cachedpos, pointlist->at(edgeIndices[edgeindex].endpts[1]).pos);
	float restlength2 = distance(cachedpos, pointlist->at(edgeIndices[edgeindex].endpts[0]).pos);
	float currlength1 = distance(pointpos, pointlist->at(edgeIndices[edgeindex].endpts[1]).pos);
	float currlength2 = distance(pointpos, pointlist->at(edgeIndices[edgeindex].endpts[0]).pos);

	push_back_tense(pointindex, edgeIndices[edgeindex].endpts[1], restlength1,currlength1);

	edgeIndices[edgeindex].endpts[1] = pointindex;
	edgeIndices[edgeindex].currentlength = currlength2;
	edgeIndices[edgeindex].restlength = restlength2;


}

void edgeList::set_cachedpoints(int i) {
	if (ce1i == i)
	{
		ce1p1 = pointlist->at(edgeIndices[ce1i].endpts[0]).pos;
		ce1p2 = pointlist->at(edgeIndices[ce1i].endpts[1]).pos;
	}
	if (ce2i == i)
	{
		ce2p1 = pointlist->at(edgeIndices[ce2i].endpts[0]).pos;
		ce2p2 = pointlist->at(edgeIndices[ce2i].endpts[1]).pos;
	}
}

void edgeList::push_back_tense(int ind1, int ind2, float restl, float tensel) {
	edgeIndices.push_back(edge{ {ind1, ind2},restl,tensel });
}

void edgeList::push_back(int ind1, int ind2) {
	edgeIndices.push_back(edge{ {ind1, ind2},
	distance(pointlist->at(ind1).pos,pointlist->at(ind2).pos)
		});
	edgeIndices.back().currentlength = edgeIndices.back().restlength;
};

void edgeList::recompute_currentlength() {
	for (int i = 0; i < (int)edgeIndices.size(); i++)
	{
		edgeIndices[i].currentlength =distance(
			pointlist->at(edgeIndices[i].endpts[0]).pos,
			pointlist->at(edgeIndices[i].endpts[1]).pos
			);
		
	}
}

bool edgeList::detect_cycles(edge e) {
	
	int* visited = new int[(int)pointlist->size()-1];
	for (int i = 0; i < (int)pointlist->size(); i++)
	{
		visited[i] = -1;
	}
	// Create a queue for BFS
	list<int> queue;
	
	visited[e.endpts[0]] = 0;


	queue.push_back(e.endpts[0]);

	list<int>::iterator i;
	int s;

	while (!queue.empty())
	{
		// Dequeue a vertex from queue and print it
		s = queue.front();
		console()<< "queue item:" << s << " ";
		queue.pop_front();
		list<int> adj = getAdjacent(s);
		for (i = adj.begin(); i != adj.end(); ++i)
		{
			console() << "adj" << s << " ";
			
			if (visited[*i]==-1)
			{
				visited[*i] = 0;
				queue.push_back(*i);
			}
			if (visited[*i] == 0) {
				e.isCyclic = true;
			}

		}
	}
	return e.isCyclic;
}

list<int> edgeList::getAdjacent(int pindex){
	list<int> adjlist;
	for (const edge& edge : edgeIndices) {
		if (edge.endpts[0] == pindex) {
			adjlist.push_back(edge.endpts[1]);
		}
		if (edge.endpts[1] == pindex) {
			adjlist.push_back(edge.endpts[0]);
		}
	}
	return adjlist;
}
