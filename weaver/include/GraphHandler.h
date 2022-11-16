#pragma once
#include "GraphSetup.h"
#include "DataContainer.h"
#include "cinder/app/App.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>

class GraphHandler
{
private:
	void setup();
	edge_ti ei, eiend;
	vertex_ti vi, viend;
	edge_t cache_edge;
	vertex_t nullVert;


public:
	GraphHandler(DataContainer& DATA) : data(DATA) { setup(); } //https://www.mygreatlearning.com/blog/constructor-in-cpp/#:~:text=What%20is%20Constructor%20in%20C%2B%2B%3F,of%20a%20class%20is%20created. , https://stackoverflow.com/questions/33260261/can-i-automatically-call-a-method-of-a-class-on-object-declaration-in-c
	edge_t connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind = 0, bool isforbidden = false);
	Graph g;
	DataContainer& data;
	std::optional<edge_t> GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g);
	void InitialWebFromObj(float rc, std::string filename);
	//ci::vec3 interpolate(cyclicedge edge, float t);
	void relaxPhysics();
};
