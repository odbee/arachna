#pragma once


#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>

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
	bool isfixed;
	vec2 pos;
};

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;

typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
typedef std::pair<EdgeIterator, EdgeIterator> EdgePair;
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
EdgePair ep;
VertexDescriptor u, v;



int nClosestEdge(vec2* mousepos, Graph g);

vec2 linecp(vec2 start, vec2 end, vec2 point);
