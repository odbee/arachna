#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct edge {
	array<int, 2> endpts;
	float restlength;
	float currentlength;
	bool isCyclic;
};
class pt {
	public:
		vec2 pos;
		bool isfixed = false;
		vec2 movevec;
};

class pointList {
	public:
		vector<pt> wPoints;

};

class edgeList {
	public:
		vector<edge> edgeIndices;
		vector<pt> *pointlist;
		int ce1i;
		vec2 ce1p1;
		vec2 ce1p2;
		int ce2i;
		vec2 ce2p1;
		vec2 ce2p2;
		vec2 cachedProjectedPt1;
		vec2 cachedProjectedPt2;
		void nconnect_point_with_edge(int pointindex, int edgeindex,vec2 cachedpos);
		void push_back(int ind1, int ind2);
		void push_back_tense(int ind1, int ind2, float restl = 0, float tensel = 0);
		void set_cachedpoints(int i);
		void recompute_currentlength();
		bool detect_cycles(edge e);
		list<int> getAdjacent(int pindex);

};



vector<edge> connect_point_with_edge(int pointindex, int edgeindex, vector<edge> edgelist);
vector<pt> animate(vector<pt> vertexlist,edgeList edgelist);
vec2 linecp(vec2 start, vec2 end, vec2 point);
pair<int, vec2> closestedge(vec2 mousepos, vector<edge> edgelist, vector<pt> veclist);
int nClosestEdge(vec2 *mousepos, vector<edge> edgelist, vector<pt> veclist);

string list_edges(vector<edge> edgelist);
string list_vertices(vector<pt> vertexlist);
