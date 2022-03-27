#pragma once
#include "cinder/app/App.h"



#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>




struct EdgeProperties
{
	float restlength;
	float currentlength;
	int index;
	bool isforbidden;
	std::vector<std::array<float, 2>> densityparams;
	float probability;
};

struct VertexProperties
{
	std::size_t index;
	bool isfixed;
	ci::vec3 pos;
	ci::vec3 movevec;
	std::vector<size_t> cycles;
};

struct GraphProperties {
	int currentindex;
};

typedef boost::adjacency_list<boost::vecS,
	boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;
Graph g;


boost::property_map< Graph, ci::vec3 VertexProperties::* >::type position = boost::get(&VertexProperties::pos, g);
boost::property_map< Graph, bool VertexProperties::* >::type fixedBool = boost::get(&VertexProperties::isfixed, g);
boost::property_map< Graph, ci::vec3 VertexProperties::* >::type moveVecPm = boost::get(&VertexProperties::movevec, g);
boost::property_map< Graph, std::vector<size_t> VertexProperties::* >::type cyclesPm = boost::get(&VertexProperties::cycles, g);


boost::property_map< Graph, float EdgeProperties::* >::type currentLengthPm = boost::get(&EdgeProperties::currentlength, g);
boost::property_map< Graph, float EdgeProperties::* >::type probabilityPm = boost::get(&EdgeProperties::probability, g);

boost::property_map< Graph, float EdgeProperties::* >::type restLengthPm = boost::get(&EdgeProperties::restlength, g);
boost::property_map< Graph, int EdgeProperties::* >::type indexPm = boost::get(&EdgeProperties::index, g);

boost::property_map< Graph, bool EdgeProperties::* >::type forbiddenPm = boost::get(&EdgeProperties::isforbidden, g);
boost::property_map< Graph, std::vector<std::array<float, 2>> EdgeProperties::* >::type densityPm = boost::get(&EdgeProperties::densityparams, g);


typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;

typedef Graph::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::edge_iterator edge_ti;

boost::graph_traits<Graph>::edge_iterator ei, eiend;


edge_t selected_edge, hovered_edge, empty_edge;
bool justclicked = false;
std::vector<std::vector<size_t>> cycles;
int cycleN;
int index_in_commoncyc = 0;
std::vector<size_t> commoncycles, empty_vector;
float f_closestDist;