#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"
#include "cinder/Log.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"
#include "cinder/gl/Texture.h"
#include "cinder/CinderImGui.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <udgcd.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;


int GLOBALINT = 1;

struct EdgeProperties
{
	float restlength;
	float currentlength;
	int index;
	bool isforbidden;
};

struct VertexProperties
{
	std::size_t index;
	bool isfixed;
	vec3 pos;
	vec3 movevec;
	std::vector<size_t> cycles;
};

struct GraphProperties {
	int currentindex;
};


typedef boost::adjacency_list<boost::vecS,
	boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;
Graph g;


boost::property_map< Graph, vec3 VertexProperties::* >::type position = boost::get(&VertexProperties::pos, g);
boost::property_map< Graph, bool VertexProperties::* >::type fixedBool = boost::get(&VertexProperties::isfixed, g);
boost::property_map< Graph, vec3 VertexProperties::* >::type moveVecPm = boost::get(&VertexProperties::movevec, g);
boost::property_map< Graph, std::vector<size_t> VertexProperties::* >::type cyclesPm = boost::get(&VertexProperties::cycles, g);


boost::property_map< Graph, float EdgeProperties::* >::type currentLengthPm = boost::get(&EdgeProperties::currentlength, g);
boost::property_map< Graph, float EdgeProperties::* >::type restLengthPm = boost::get(&EdgeProperties::restlength, g);
boost::property_map< Graph, int EdgeProperties::* >::type indexPm = boost::get(&EdgeProperties::index, g);

boost::property_map< Graph, bool EdgeProperties::* >::type forbiddenPm = boost::get(&EdgeProperties::isforbidden, g);


boost::graph_traits<Graph>::edge_iterator ei, eiend;
boost::graph_traits< Graph >::vertex_iterator vi, viend;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;

typedef Graph::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::edge_iterator edge_ti;

std::vector<std::vector<size_t>> cycles;
edge_t cache_edge;
boost::graph_traits<Graph>::vertex_descriptor null = boost::graph_traits<Graph>::null_vertex();
// null edge