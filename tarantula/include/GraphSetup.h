#ifndef GRAPHSETUP_H_INCLUDED
#define GRAPHSETUP_H_INCLUDED

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

struct EdgeProperties
{
	float restlength;
	float currentlength;
};

struct VertexProperties
{
	std::size_t index;
	bool isfixed;
	vec3 pos;
	vec3 movevec;
};
typedef boost::adjacency_list<boost::listS,
	boost::listS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;
Graph g;


boost::property_map< Graph, vec3 VertexProperties::* >::type position = boost::get(&VertexProperties::pos, g);
boost::property_map< Graph, bool VertexProperties::* >::type fixedBool = boost::get(&VertexProperties::isfixed, g);
boost::property_map< Graph, vec3 VertexProperties::* >::type moveVecPm = boost::get(&VertexProperties::movevec, g);


boost::property_map< Graph, float EdgeProperties::* >::type currentLengthPm = boost::get(&EdgeProperties::currentlength, g);
boost::property_map< Graph, float EdgeProperties::* >::type restLengthPm = boost::get(&EdgeProperties::restlength, g);


boost::graph_traits<Graph>::edge_iterator ei, eiend;
boost::graph_traits< Graph >::vertex_iterator vi, viend;

void connectAB(Graph* g, Graph::vertex_descriptor endPointA, Graph::vertex_descriptor endPointB, float rc) {
	float dd = distance(position[endPointA], position[endPointB]);
	Graph::edge_descriptor edge = boost::add_edge(endPointA, endPointB, *g).first;
	currentLengthPm[edge] = dd;
	restLengthPm[edge] = dd * rc;
	return;
}


void setInitialWeb(Graph* g,float rc) {
	auto a = boost::add_vertex(*g);
	position[a] = vec3(-5, -1, 10);
	fixedBool[a] = true;
	auto b = boost::add_vertex(*g);
	position[b] = vec3(15, 10, 0);
	fixedBool[b] = true;

	auto c = boost::add_vertex(*g);
	position[c] = vec3(10, -10, 20);
	fixedBool[c] = true;

	auto d = boost::add_vertex(*g);
	position[d] = vec3(0, 0, 0);
	fixedBool[d] = false;

	auto e = boost::add_vertex(*g);
	position[e] = vec3(-30, -40, 0);
	fixedBool[e] = true;


	connectAB(g, a, d, rc);
	connectAB(g, b, d, rc);
	connectAB(g, c, d, rc);
	connectAB(g, e, d, rc);

}

void addRandomEdge(Graph* g, float rc) {
	tie(ei, eiend) = boost::edges(*g);
	int iteratorLength = 0;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		iteratorLength++;
	}
	auto e1 = boost::add_vertex(*g);
	fixedBool[e1] = false;
	auto e2 = boost::add_vertex(*g);
	fixedBool[e2]= false;
	//created 2 points

	ei = boost::edges(*g).first;
	auto randiter = rand() % iteratorLength;
	for (size_t i = 0; i < randiter; i++)
	{
		ei++;
	}
	//gotten iterator from random iterator 1
	
	auto eii = ei;
	auto tp1 = position[source(*ei, *g)];
	auto tp2 = position[target(*ei, *g)];
	// taken positions of edge points from iterator edge
	auto randiter2 = rand() % iteratorLength;
	position[e1]= tp1 + (tp2 - tp1) * float(float(rand() % 1000) / 1000);
	//set position of edge point 1 from random point on edge
	while (randiter == randiter2)
	{
		randiter2 = rand() % iteratorLength;
	}
	
	ei = boost::edges(*g).first;
	for (size_t i = 0; i < randiter2; i++)
	{
		ei++;
	}

	//gotten iterator from random iterator 2 
	tp1 = position[source(*ei, *g)];
	tp2 = position[target(*ei, *g)];
	position[e2] = tp1 + (tp2 - tp1) * float(float(rand() % 1000) / 1000);
	//set position of edge point 2 from random point on edge
	connectAB(g, e1, e2, rc);
	connectAB(g, e2, source(*ei, *g), rc);
	connectAB(g, e2, target(*ei, *g), rc);
	connectAB(g, e1, source(*eii, *g), rc);
	connectAB(g, e1, target(*eii, *g), rc);
	boost::remove_edge(*eii, *g);
	boost::remove_edge(*ei, *g);
}
void relaxPhysics(Graph* g) {
	float k = 1.1f;
	float eps = 0.1f;
	Graph::vertex_descriptor v1, v2;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, *g);
		v2 = boost::target(*ei, *g);
		if (length(position[v2] - position[v1]) > 0.01) {
			if (!fixedBool[v1]) {
				moveVecPm[v1] += 1 * k * (currentLengthPm[*ei] - restLengthPm[*ei]) *
					normalize(position[v2] - position[v1]);
			}
			if (!fixedBool[v2]) {
				moveVecPm[v2] += 1 * k * (currentLengthPm[*ei] - restLengthPm[*ei]) *
					normalize(position[v1] - position[v2]);
			}
		}
	}

	for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {

		//position[*vi] += moveVecPm[*vi];
		position[*vi] += eps * moveVecPm[*vi];
		moveVecPm[*vi] = vec3(0, 0, 0);
	}
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, *g);
		v2 = boost::target(*ei, *g);
		currentLengthPm[*ei] = distance(position[v1], position[v2]);
	}
}
void drawPoints(Graph* g, mat4 proj, vec4 viewp, bool drawNumbers = true, bool drawVertexPoints = true) {
	
	if (drawNumbers) {
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);
			vec3 danchorp = position[boost::source(*ei, *g)] + (position[boost::target(*ei, *g)] - position[boost::source(*ei, *g)]) * 0.5f;
			vec2 anchorp1 = glm::project(danchorp, mat4(), proj, viewp);
			gl::drawString(to_string(currentLengthPm[*ei] / restLengthPm[*ei]), anchorp1);
		}
	}
	if (drawVertexPoints)
	{
		for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {

			gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
			vec2 anchorp1 = glm::project(position[*vi], mat4(), proj, viewp);
			gl::drawSolidCircle(anchorp1, 3);
			//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
		}
	}
	
}
void drawGraph(Graph *g, mat4 proj, vec4 viewp) {
	gl::ScopedColor color(Color::gray(0.2f));
	gl::color(1.0f, 1.0f, 1.0f, 0.8f);
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);
	}
}
#endif // GRAPHSETUP
