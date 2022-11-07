#pragma once
#include "GraphSetup.h"
#include "VoxelHandler.h"
#include "AnchorPointsHandler.h"

#include "cinder/app/App.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include "HelperFunctions.h"

class GraphHandler
{
private:
	void setup();
public:
	GraphHandler(VoxelHandler& VH, AnchorPointsHandler& AH) :vh(VH), ah(AH) { setup(); } //https://www.mygreatlearning.com/blog/constructor-in-cpp/#:~:text=What%20is%20Constructor%20in%20C%2B%2B%3F,of%20a%20class%20is%20created. , https://stackoverflow.com/questions/33260261/can-i-automatically-call-a-method-of-a-class-on-object-declaration-in-c

	float G_density = 0.0005f, G_length = .999f, G_tension = 0.0005f;
	std::vector<float*> vallist{ &G_density,&G_length,&G_tension };
	bool forbiddenCheck;
	int GLOBALINT = 1;
	int currentindex;

	edge_ti ei, eiend;
	vertex_ti vi, viend;
	std::vector<std::vector<size_t>> cycles;
	edge_t cache_edge;
	vertex_t nullVert;
	Graph g;

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

	VoxelHandler& vh;
	AnchorPointsHandler& ah;
	void addCyclesToVertices();
	edge_t connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind = 0, bool isforbidden = false);
	void emptyCyclesFromVertices();
	void exportGraph(std::string directory);
	void exportGraphOBJ(Graph g, std::string directory);
	float getDivPoint(edge_t edge);
	std::optional<edge_t> GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g);
	template <typename T>
	edge_t getRandomEdgeFromEdgeListIntegrated(T& begin, T& end, bool forbcheck = false);
	float integrateEdge(edge_t edge);
	void InitialWebFromObj(float rc, std::string filename, std::vector<std::vector<size_t>>& cycs);
	void addIntersectionVertexToCycles(cyclicedge& edge, int cycleIndex, std::vector<std::vector<size_t>>* cycs);

	void addRandomCyclicEdgeTesting(float rc, std::vector<std::vector<size_t>>* cycs);

	std::vector<edge_t> connectEdges(cyclicedge startedge, cyclicedge goaledge, float rc);
	std::vector<edge_t> getConnectedEdges(cyclicedge edge, std::vector<size_t>& edgeindices, bool forbCheck);
	void initEdge(edge_t ed, cyclicedge& cedge);
	ci::vec3 interpolate(cyclicedge edge, float t);
	void removebyindex(std::vector<size_t>& vec, size_t ind);
	void resolveIntersections(cyclicedge start, cyclicedge goal, size_t cycle1index, size_t cycle2index);
	void shiftCycle(std::vector<size_t>& cycle, int index1, int  index2);
	void updatetext(std::string in);
	std::vector<size_t> vectorcycles(std::vector<size_t> vec);
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
	void addparam(int index, std::vector<std::array<float, 2>>& paramlist, ci::vec3& currentvoxel, VoxelHandler vh);
	bool sortbystartvoxeldist(const coordcont& x, const coordcont& y);
	bool operator < (const GraphHandler::coordcont& str) const
	{
		return (startvoxeldist < str.startvoxeldist);
	}
};

