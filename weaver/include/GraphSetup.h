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
	float densityval;
};

struct VertexProperties
{
	std::size_t index;
	bool isfixed;
	ci::vec3 pos;
	ci::vec3 movevec;
	std::vector<size_t> cycles;
};

typedef boost::adjacency_list<boost::vecS,
	boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;

typedef boost::graph_traits<Graph>::vertex_iterator vertex_ti;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::edge_iterator edge_ti;
typedef std::map<std::array<signed int, 3>, float> voxel_map;
typedef std::vector<ci::vec3> anchor_points;
