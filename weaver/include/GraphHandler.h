#pragma once
#include "GraphSetup.h"
#include "DataContainer.h"
#include "cinder/app/App.h"
#include <fstream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include "cinder/Ray.h"
class GraphHandler
{
private:
	void setup();
	edge_ti ei, eiend;
	vertex_ti vi, viend;
	edge_t cache_edge;
	vertex_t nullVert;
	std::vector<EdgeContainer> edgeInfo;
	std::map<int, EdgeContainer> edgeMap;

	std::vector<RecipeContainer> recipeInfo;
	std::vector<VertexContainer> vertexInfo;
	std::string edgesFilename;
	std::string recipeFilename;
	std::string verticesFilename;
	
public:
	GraphHandler(DataContainer& DATA) : data(DATA) { setup(); } //https://www.mygreatlearning.com/blog/constructor-in-cpp/#:~:text=What%20is%20Constructor%20in%20C%2B%2B%3F,of%20a%20class%20is%20created. , https://stackoverflow.com/questions/33260261/can-i-automatically-call-a-method-of-a-class-on-object-declaration-in-c
	std::vector<RecipeContainer> getRecipeInfo();
	edge_t connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind = 0, bool isforbidden = false);
	Graph g;
	DataContainer& data;
	std::optional<edge_t> GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g);
	std::vector<EdgeContainer> loadEdges(std::string fname);
	std::vector<RecipeContainer> loadRecipe(std::string fname);
	std::vector<VertexContainer> loadVertices(std::string fname);
	void addEdgeFromRecipe(int index);
	void removeEdgeFromRecipe(int index);
	void InitialWebFromObj(float rc, std::string filename);
	void AddAllVerts();
	ci::vec3 interpolate(ci::vec3 start, ci::vec3 end, float t);
	void relaxPhysics();
	void updateEdge( edge_t edge_desc, EdgeContainer edge_cont);
	edge_t getClosestEdgeFromRay(ci::Ray ray);
	float determinant(ci::vec3 a, ci::vec3 b, ci::vec3 c);
	float GraphHandler::closestDist(ci::vec3 a0, ci::vec3 a1, ci::vec3 b0, ci::vec3 b1, ci::vec3& firstpoint, ci::vec3& secondpoint, bool clampAll = false, bool clampA0 = false, bool clampA1 = false, bool clampB0 = false, bool clampB1 = false);

	
};

