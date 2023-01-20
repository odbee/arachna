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
#include <numeric>
#include "VERBOSITYLEVEL.h"
#include <boost/bind/bind.hpp>


class GraphHandler
{
private:
	void setup();
	edge_ti ei, eiend;
	vertex_ti vi, viend;
	edge_t cache_edge;
	vertex_t nullVert;
	std::vector<EdgeContainer> edgeInfo;

	std::vector<RecipeContainer> recipeInfo;
	std::vector<VertexContainer> vertexInfo;
	std::string edgesFilename;
	std::string recipeFilename;
	std::string verticesFilename;
	

	
	
public:
	GraphHandler(DataContainer& DATA) : data(DATA) { setup(); } //https://www.mygreatlearning.com/blog/constructor-in-cpp/#:~:text=What%20is%20Constructor%20in%20C%2B%2B%3F,of%20a%20class%20is%20created. , https://stackoverflow.com/questions/33260261/can-i-automatically-call-a-method-of-a-class-on-object-declaration-in-c
	struct originalEdge {
		std::vector<int> listOfVertices;

		std::vector<EdgeContainer> listOfEdges;
		std::vector<int> listOfEdgeInds;
		int startVert;
		int endVert;
		bool isDrawn = false;
		std::string printEdges() {
			std::stringstream text;
			text << "{ " ;
			for (auto& x : listOfEdges) {
				text << x.uniqueIndex << " , ";
			}
			text.seekp(-2, std::ios_base::end);

			text << "} ";
			return text.str();
		};

		std::vector<EdgeContainer> findVertexInEdges(int vert) {
			std::vector< EdgeContainer > containingEdges;
			std::vector< EdgeContainer >::iterator it;
			it = listOfEdges.begin();
			it = std::find_if(it, listOfEdges.end(), boost::bind(&EdgeContainer::sourceV, boost::placeholders::_1) == vert);
			if (it != listOfEdges.end()) {
				ci::app::console() << it->uniqueIndex << std::endl;
				containingEdges.push_back(*it);
			}
			while (it != listOfEdges.end()) {
				it = std::find_if(it+1, listOfEdges.end(), boost::bind(&EdgeContainer::sourceV, boost::placeholders::_1) == vert);
				if (it != listOfEdges.end()) {
					ci::app::console() << it->uniqueIndex << std::endl;
					containingEdges.push_back(*it);
				}
			}
			it = listOfEdges.begin();
			it = std::find_if(it, listOfEdges.end(), boost::bind(&EdgeContainer::targetV, boost::placeholders::_1) == vert);
			if (it != listOfEdges.end()) {
				ci::app::console() << it->uniqueIndex << std::endl;

				containingEdges.push_back(*it);
			}
			while (it != listOfEdges.end()) {
				it = std::find_if(it+1, listOfEdges.end(), boost::bind(&EdgeContainer::targetV, boost::placeholders::_1) == vert);
				if (it != listOfEdges.end()) {
					ci::app::console() << it->uniqueIndex << std::endl;
					
					containingEdges.push_back(*it);
				}
			}
			return containingEdges;
			
		};
		std::string printEdgeVerts() {
			std::stringstream text;
			text << "{ ";
			for (auto& x : listOfEdges) {
				text << "("<< x.sourceV <<"," << x.targetV << ")" << " , ";
			}
			text.seekp(-2, std::ios_base::end);

			text << "} ";
			return text.str();
		};
		void calculateVertexSeries() {

		};
		std::vector<EdgeContainer> returnInbetweenEdges(int start, int end) {
			ci::app::console() << "list:" << stringfromVec(listOfVertices) << std::endl;
			ci::app::console() << "startVert:" << start << ", endVert:" << end << std::endl;

			int startdist = std::distance(listOfVertices.begin(), std::find(listOfVertices.begin(), listOfVertices.end(), start));
			int enddist = std::distance(listOfVertices.begin(), std::find(listOfVertices.begin(), listOfVertices.end(), end));
			if (startdist > enddist) {
				std::swap(startdist, enddist);
			}
			ci::app::console() << "startdist: " << startdist << "enddist:" << enddist << std::endl;
			std::vector<EdgeContainer> result;
			ci::app::console() << "list of edges:" << printEdges() << std::endl;

			if (enddist < listOfVertices.size() - 1) {
					result = std::vector<EdgeContainer>{ listOfEdges.begin() + startdist, listOfEdges.begin() + enddist  };
					ci::app::console() << "start item:" << (listOfEdges.begin() + startdist)->uniqueIndex << ", end item:" << (listOfEdges.begin() + enddist - 1)->uniqueIndex << std::endl;
					ci::app::console() << "created inbetween edges of size " << result.size() << std::endl;

			}
			else {
				result = std::vector<EdgeContainer>{ listOfEdges.begin() + startdist, listOfEdges.end() };
			}
			
			if (startdist > enddist) {
				std::reverse(result.begin(), result.end());// REVERSING THE RESULT AFTER SWAP
			}

			return result;
		};
	};
	std::map<int,originalEdge> originalEdges;
	std::map<int, EdgeContainer> edgeMap;

	std::vector<RecipeContainer> getRecipeInfo();
	edge_t connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind = 0, bool isforbidden = false);
	edge_t weaverConnect(vertex_t endPointA, vertex_t endPointB, EdgeContainer edgeInfo);
	Graph g;
	EdgesGraph edgesG;
	DataContainer& data;
	std::optional<edge_t> GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g);
	std::vector<EdgeContainer> loadEdges(std::string fname);
	std::vector<RecipeContainer> loadRecipe(std::string fname);
	std::vector<VertexContainer> loadVertices(std::string fname);
	void computeCurrentLength();
	void addEdgeFromRecipe(int index);
	void removeEdgeFromRecipe(int index);
	void InitialWebFromObj(float rc, std::string filename);
	void AddAllVerts();
	ci::vec3 interpolate(ci::vec3 start, ci::vec3 end, float t);
	void relaxPhysics();
	void updateEdge( edge_t edge_desc, EdgeContainer edge_cont);
	edge_t getClosestEdgeFromRay(ci::Ray ray);
	float determinant(ci::vec3 a, ci::vec3 b, ci::vec3 c);
	float closestDist(ci::vec3 a0, ci::vec3 a1, ci::vec3 b0, ci::vec3 b1, ci::vec3& firstpoint, ci::vec3& secondpoint, bool clampAll = false, bool clampA0 = false, bool clampA1 = false, bool clampB0 = false, bool clampB1 = false);
	void initOriginalEdges();
	void printOriginalEdges();
	//void printoriginalEdges();
	void setupEdgesGraph();
	void calculateDivEdgeLengths(int edgeInt);
};

