#pragma once
#include "GraphSetup.h"
#include "DataContainer.h"
#include "cinder/app/App.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include "HelperFunctions.h"

class GraphHandler
{
private:
	void setup();
	edge_ti ei, eiend;
	vertex_ti vi, viend;
	edge_t cache_edge;
	vertex_t nullVert;
	struct coordcont;
	struct cyclicvert {
		size_t index;
		ci::vec3 pos;

	};

	struct cyclicedge {
		edge_ti ei_iterator;
		edge_t descriptor;
		cyclicvert start;
		cyclicvert end;
		std::vector<size_t> cycles;
		vertex_t divisionvert;
	};
public:
	GraphHandler(DataContainer& DATA) : data(DATA) { setup(); } //https://www.mygreatlearning.com/blog/constructor-in-cpp/#:~:text=What%20is%20Constructor%20in%20C%2B%2B%3F,of%20a%20class%20is%20created. , https://stackoverflow.com/questions/33260261/can-i-automatically-call-a-method-of-a-class-on-object-declaration-in-c








	DataContainer& data;
	Graph& g = data.graph;
	cycleList& cycles = data.cycs;
	voxel_map& voxelMap = data.voxels;
	bool& forbiddenCheck = data.forbiddenCheck;
	int& GLOBALINT = data.GLOBALINT;


	void addCyclesToVertices();
	edge_t connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind = 0, bool isforbidden = false);
	void emptyCyclesFromVertices();
	void exportGraph(std::string directory);
	void exportGraphOBJ(Graph g, std::string directory);
	float getDivPoint(edge_t edge);
	std::optional<edge_t> GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g);
	template <typename T>
	edge_t getRandomEdgeFromEdgeListIntegrated(T& begin, T& end, bool forbcheck, float density, float length, float tension);
	float integrateEdge(edge_t edge, voxel_map voxelMap);
	void InitialWebFromObj(float rc, std::string filename);
	void addIntersectionVertexToCycles(cyclicedge& edge, int cycleIndex);

	void addRandomCyclicEdgeTesting(float rc);

	std::vector<edge_t> connectEdges(cyclicedge startedge, cyclicedge goaledge, float rc);
	std::vector<edge_t> getConnectedEdges(cyclicedge edge, std::vector<size_t>& edgeindices, bool forbCheck);
	void initEdge(edge_t ed, cyclicedge& cedge);
	ci::vec3 interpolate(cyclicedge edge, float t);
	void removebyindex(std::vector<size_t>& vec, size_t ind);
	void resolveIntersections(cyclicedge start, cyclicedge goal, size_t cycle1index, size_t cycle2index);
	void shiftCycle(std::vector<size_t>& cycle, int index1, int  index2);
	void updatetext(std::string in);
	std::vector<size_t> vectorcycles(std::vector<size_t> vec);
	void relaxPhysics();
	
	edge_t pickrandomEdge(edge_ti& begin, edge_ti& end);
};

struct GraphHandler::coordcont {
	float start;
	float end;
	ci::vec3 axis;
	signed int sign;
	int startvoxel;
	int endvoxel;
	float startvoxeldist;
	float direction;

	void calcvals();
	bool compareendvoxels();
	void addparam(int index, std::vector<std::array<float, 2>>& paramlist, ci::vec3& currentvoxel, voxel_map vh);
	bool sortbystartvoxeldist(const coordcont& x, const coordcont& y);
	bool operator < (const GraphHandler::coordcont& str) const
	{
		return (startvoxeldist < str.startvoxeldist);
	}
};

