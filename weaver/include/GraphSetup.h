#pragma once

#include "cinder/app/App.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>


struct EdgeProperties
{
	int uniqueIndex;
	float restlength;
	float currentlength;
	int index;
	bool isforbidden;
};

struct EdgeContainer
{
	int uniqueIndex;
	int index;
	float restlength;
	int isForbidden;
	int sourceV;
	int targetV;
	
};

struct RecipeContainer {
	int OldEdgeA, OldEdgeB;
	int NewEdgeA1, NewEdgeA2, NewEdgeB1, NewEdgeB2;
	int NewEdgeC;
	float posA1, posA2;
};
struct VertexContainer {
	int index;
	bool isfixed;
	ci::vec3 pos;
};


struct VertexProperties
{
	std::size_t index;
	bool isfixed;
	ci::vec3 pos;
	ci::vec3 movevec;
};

typedef boost::adjacency_list<boost::vecS,
	boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;

typedef boost::graph_traits<Graph>::vertex_iterator vertex_ti;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::edge_iterator edge_ti;
typedef std::map<std::array<signed int, 3>, float> voxel_map;
typedef std::vector<ci::vec3> anchor_points;
