
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "spiderprimitives.h"

vector<edge> connect_point_with_edge(int pointindex, int edgeindex, vector<edge> edgelist) {
	edgelist.push_back(edge{ pointindex,edgelist[edgeindex].endpts[1] });
	edgelist[edgeindex].endpts[1] = pointindex;
	return edgelist;
}



void animate(vector<pt> vertexlist, vector<edge> edgelist) {
	vector<pt> copyverts = vertexlist;
	int counter = 0;
	for (const pt& point : vertexlist) {

		//copyverts[counter]=point+
		counter++;
	}

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

void edgeList::push_back_tense(int ind1, int ind2, float restl = 0, float tensel = 0) {
	edgeIndices.push_back(edge{ {ind1, ind2},restl,tensel });
}

void edgeList::push_back(int ind1, int ind2) {
	edgeIndices.push_back(edge{ {ind1, ind2},
	distance(pointlist->at(ind1).pos,pointlist->at(ind2).pos)
		});
	edgeIndices.back().currentlength = edgeIndices.back().restlength;
};