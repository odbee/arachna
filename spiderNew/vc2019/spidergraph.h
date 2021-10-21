#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

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


boost::property_map< Graph, float EdgeProperties::* >::type currentLengthPm= boost::get(&EdgeProperties::currentlength, g);
boost::property_map< Graph, float EdgeProperties::* >::type restLengthPm = boost::get(&EdgeProperties::restlength, g);


boost::graph_traits<Graph>::edge_iterator ei, eiend;
boost::graph_traits< Graph >::vertex_iterator vi, viend;

void connectTwoPointsAndSetupTension(Graph::vertex_descriptor endPointA, Graph::vertex_descriptor endPointB) {
	float dd = distance(position[endPointA], position[endPointB]);
	Graph::edge_descriptor edge = boost::add_edge(endPointA, endPointB, g).first;
	currentLengthPm[edge] = dd;
	restLengthPm[edge] = dd * 0.7;;


	return;
}