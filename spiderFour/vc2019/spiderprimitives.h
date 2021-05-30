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
	float tension = 1;
};
struct pt {
	vec2 pos;
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
		void push_back(int ind1,int ind2) {
			edgeIndices.push_back(edge{ {ind1, ind2},
			distance(pointlist->at(ind1).pos,pointlist->at(ind2).pos)
				});
			edgeIndices.back().currentlength = edgeIndices.back().restlength;
		};
		void push_back_tense(int ind1, int ind2, float restl = 0, float tensel = 0) {
			edgeIndices.push_back(edge{ {ind1, ind2},restl,tensel });
		}

		void set_cachedpoints(int i) {
			if (ce1i ==i)
			{
				ce1p1 = pointlist->at(edgeIndices[ce1i].endpts[0]).pos;
				ce1p2= pointlist->at(edgeIndices[ce1i].endpts[1]).pos;
			}
			if (ce2i == i)
			{

				ce2p1 = pointlist->at(edgeIndices[ce2i].endpts[0]).pos;
				ce2p2 = pointlist->at(edgeIndices[ce2i].endpts[1]).pos;
			}

		}

};



vector<edge> connect_point_with_edge(int pointindex, int edgeindex, vector<edge> edgelist);
pair<vector<pt>, vector<edge>> animate(vector<pt> vertexlist, vector<edge> edgelist);
vec2 linecp(vec2 start, vec2 end, vec2 point);
pair<int, vec2> closestedge(vec2 mousepos, vector<edge> edgelist, vector<pt> veclist);
int nClosestEdge(vec2 *mousepos, vector<edge> edgelist, vector<pt> veclist);

string list_edges(vector<edge> edgelist);
string list_vertices(vector<pt> vertexlist);
