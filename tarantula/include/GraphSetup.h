#ifndef GRAPHSETUP_H_INCLUDED
#define GRAPHSETUP_H_INCLUDED

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <udgcd.hpp>


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
	std::size_t index;
	bool isfixed;
	vec3 pos;
	vec3 movevec;
	std::vector<size_t> cycles;
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


boost::graph_traits<Graph>::edge_iterator ei, eiend;
boost::graph_traits< Graph >::vertex_iterator vi, viend;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;

typedef Graph::edge_descriptor edge_t;

void connectAB(Graph* g, Graph::vertex_descriptor endPointA, Graph::vertex_descriptor endPointB, float rc) {
	float dd = distance(position[endPointA], position[endPointB]);
	Graph::edge_descriptor edge = boost::add_edge(endPointA, endPointB, *g).first;
	currentLengthPm[edge] = dd;
	restLengthPm[edge] = dd * rc;
	return;
}

edge_t my_find_edge(vertex_t v, vertex_t u, Graph const& g)
{
	for (auto e : boost::make_iterator_range(out_edges(v, g))) {
		if (target(e, g) == u)
			return e;
	}
	throw std::domain_error("my_find_edge: not found");
}

string getCyclesString(std::vector<size_t> cycleslist) {
	string outtext;
	for (const auto& elem : cycleslist) {
		outtext.append(to_string(elem) + "  ");

	}
	return outtext;
}

void setInitialWeb(Graph* g,float rc) {
	auto a = boost::add_vertex(*g);
	position[a] = vec3(-5, -1, 10);
	fixedBool[a] = true;
	auto b = boost::add_vertex(*g);
	position[b] = vec3(15, 10, 0);
	fixedBool[b] = true;

	auto c = boost::add_vertex(*g);
	position[c] = vec3(10, -10, 20);
	fixedBool[c] = true;

	auto d = boost::add_vertex(*g);
	position[d] = vec3(0, 0, 0);
	fixedBool[d] = false;

	auto e = boost::add_vertex(*g);
	position[e] = vec3(-30, -40, 0);
	fixedBool[e] = true;


	connectAB(g, a, d, rc);
	connectAB(g, b, d, rc);
	connectAB(g, c, d, rc);
	connectAB(g, e, d, rc);

}

void addCyclesToVertices(Graph* g, std::vector<std::vector<size_t>> cycles) {
	for (int i = 0; i < cycles.size(); i++)
	{
		for (const auto& elem : cycles[i])
			cyclesPm[elem].push_back(i);
	}
}

void addRandomEdge(Graph* g, float rc) {
	tie(ei, eiend) = boost::edges(*g);
	int iteratorLength = 0;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		iteratorLength++;
	}
	auto v_insert1 = boost::add_vertex(*g);
	fixedBool[v_insert1] = false;
	auto v_insert2 = boost::add_vertex(*g);
	fixedBool[v_insert2]= false;
	//created 2 points

	ei = boost::edges(*g).first;
	auto randiter = rand() % iteratorLength;
	for (size_t i = 0; i < randiter; i++)
	{
		ei++;
	}
	//gotten iterator from random iterator 1
	
	auto eii = ei;
	auto p_edgeVertex1 = position[source(*ei, *g)];
	auto p_edgeVertex2 = position[target(*ei, *g)];
	// taken positions of edge points from iterator edge
	auto randiter2 = rand() % iteratorLength;
	position[v_insert1]= p_edgeVertex1 + (p_edgeVertex2 - p_edgeVertex1) * float(float(rand() % 1000) / 1000);
	//set position of edge point 1 from random point on edge
	while (randiter == randiter2)
	{
		randiter2 = rand() % iteratorLength;
	}
	
	ei = boost::edges(*g).first;
	for (size_t i = 0; i < randiter2; i++)
	{
		ei++;
	}

	//gotten iterator from random iterator 2 
	p_edgeVertex1 = position[source(*ei, *g)];
	p_edgeVertex2 = position[target(*ei, *g)];
	position[v_insert2] = p_edgeVertex1 + (p_edgeVertex2 - p_edgeVertex1) * float(float(rand() % 1000) / 1000);
	//set position of edge point 2 from random point on edge
	connectAB(g, v_insert1, v_insert2, rc);
	connectAB(g, v_insert2, source(*ei, *g), rc);
	connectAB(g, v_insert2, target(*ei, *g), rc);
	connectAB(g, v_insert1, source(*eii, *g), rc);
	connectAB(g, v_insert1, target(*eii, *g), rc);
	boost::remove_edge(*eii, *g);
	boost::remove_edge(*ei, *g);
}


//https://stackoverflow.com/questions/5225820/compare-two-vectors-c
// rethink usage of const vector<size_t>
vector<size_t> compareVectors(const vector<size_t> vec1, const vector<size_t> vec2) {
	std::set<size_t> s1(vec1.begin(), vec1.end());
	std::set<size_t> s2(vec2.begin(), vec2.end());
	std::vector<size_t> v3;
	std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(v3));
	return v3;
}


void addRandomCyclicEdge(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs) {
	tie(ei, eiend) = boost::edges(*g);
	
	int iteratorLength = 0;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		iteratorLength++;
	auto v_insert1 = boost::add_vertex(*g);
	fixedBool[v_insert1] = false;
	auto v_insert2 = boost::add_vertex(*g);
	fixedBool[v_insert2] = false;
	//created 2 points

	auto e_startEdge = boost::edges(*g).first;
	auto randiter = rand() % iteratorLength;
	for (size_t i = 0; i < randiter; i++)
	{
		e_startEdge++;
	}
	//gotten iterator from random iterator 1

	vector<size_t> adjacentCycles= compareVectors(cyclesPm[source(*ei, *g)], cyclesPm[target(*ei, *g)]);

	auto eii = e_startEdge;
	auto p_edgeVertex1 = position[source(*e_startEdge, *g)];
	auto p_edgeVertex2 = position[target(*e_startEdge, *g)];
	size_t i_firstVertexIndex= source(*e_startEdge, *g);
	size_t i_secondVertexIndex = target(*e_startEdge, *g);

	position[v_insert1] = p_edgeVertex1 + (p_edgeVertex2 - p_edgeVertex1) * float(float(rand() % 1000) / 1000);
	// taken positions of edge points from iterator edge
	int seconditer;
	edge_t e_goalEdge;

	if (adjacentCycles.size()) {

		//zwischen hier
		int whichAdjacentCycle = rand() % (adjacentCycles.size());
		size_t cycleIndex = adjacentCycles[whichAdjacentCycle];
		vector<size_t> currentcycle= cycs->at(cycleIndex);

		auto p = std::find(currentcycle.begin(), currentcycle.end(), i_firstVertexIndex);
		auto q = std::find(currentcycle.begin(), currentcycle.end(), i_secondVertexIndex);
		auto cyclesize= currentcycle.size();

		if (p + 1 == q) {
			rotate(currentcycle.begin(), q, currentcycle.end());
		}
		else if (q + 1 == p) {
			rotate(currentcycle.begin(), q, currentcycle.end());
		}

		int randomIndexinCycle = (rand() % (currentcycle.size() - 2)) +1;
		
		size_t vertexIndex1 = currentcycle[randomIndexinCycle];
		size_t vertexIndex2 = currentcycle[randomIndexinCycle+1];
		// und hier
		
		vector<size_t> left(currentcycle.begin(), currentcycle.begin() + randomIndexinCycle);
		vector<size_t> right(currentcycle.begin() + randomIndexinCycle, currentcycle.end());
		cycs->at(0) = left;
		cycs->push_back(right);
		size_t lastindex = cycs->size() - 1;

		std::replace(cyclesPm[i_firstVertexIndex].begin(), cyclesPm[i_firstVertexIndex].end(),cycleIndex,lastindex);
		std::replace(cyclesPm[randomIndexinCycle].begin(), cyclesPm[randomIndexinCycle].end(), cycleIndex, lastindex);
		cyclesPm[v_insert1].push_back(cycleIndex);
		cyclesPm[v_insert1].push_back(lastindex);
		cyclesPm[v_insert2].push_back(cycleIndex);
		cyclesPm[v_insert2].push_back(lastindex);
		//if (p + 1 == q) {
		//	std::replace(cyclesPm[i_firstVertexIndex].begin(), cyclesPm[i_firstVertexIndex].end(), cycleIndex, lastindex);
		//	std::replace(cyclesPm[randomIndexinCycle].begin(), cyclesPm[randomIndexinCycle].end(), cycleIndex, lastindex);

		//	cycs[cycleIndex].push_back();
		//	cycs[cycleIndex].push_back();
		//	cycs[lastindex].push_back();
		//	cycs[lastindex].push_back();
		//}
		//else if (q + 1 == p) {
		//	std::replace(cyclesPm[i_firstVertexIndex].begin(), cyclesPm[i_firstVertexIndex].end(), cycleIndex, lastindex);
		//	std::replace(cyclesPm[randomIndexinCycle].begin(), cyclesPm[randomIndexinCycle].end(), cycleIndex, lastindex);

		//	cycs[cycleIndex].push_back();
		//	cycs[cycleIndex].push_back();
		//	cycs[lastindex].push_back();
		//	cycs[lastindex].push_back();
		//}
		//cycs[cycleIndex].push_back();
		//cycs[cycleIndex].push_back();
		//cycs[lastindex].push_back();
		//cycs[lastindex].push_back();


		e_goalEdge = my_find_edge(vertexIndex1, vertexIndex2, *g);


	}
	else {
		seconditer = rand() % iteratorLength;
		position[v_insert1] = p_edgeVertex1 + (p_edgeVertex2 - p_edgeVertex1) * float(float(rand() % 1000) / 1000);
		//set position of edge point 1 from random point on edge

		while (randiter == seconditer)
		{
			seconditer = rand() % iteratorLength;
		}
		ei = boost::edges(*g).first;

		for (size_t i = 0; i < seconditer; i++)
		{
			ei++;
		}
		e_goalEdge = *ei;
	}



	//gotten iterator from random iterator 2 
	p_edgeVertex1 = position[source(e_goalEdge, *g)];
	p_edgeVertex2 = position[target(e_goalEdge, *g)];
	position[v_insert2] = p_edgeVertex1 + (p_edgeVertex2 - p_edgeVertex1) * float(float(rand() % 1000) / 1000);
	//set position of edge point 2 from random point on edge
	connectAB(g, v_insert1, v_insert2, rc);
	connectAB(g, v_insert2, source(e_goalEdge, *g), rc);
	connectAB(g, v_insert2, target(e_goalEdge, *g), rc);
	connectAB(g, v_insert1, source(*eii, *g), rc);
	connectAB(g, v_insert1, target(*eii, *g), rc);
	boost::remove_edge(*eii, *g);
	boost::remove_edge(e_goalEdge, *g);
}


void relaxPhysics(Graph* g) {
	float k = 1.1f;
	float eps = 0.1f;
	Graph::vertex_descriptor v1, v2;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, *g);
		v2 = boost::target(*ei, *g);
		if (length(position[v2] - position[v1]) > 0.01) {
			if (!fixedBool[v1]) {
				moveVecPm[v1] += 1 * k * (currentLengthPm[*ei] - restLengthPm[*ei]) *
					normalize(position[v2] - position[v1]);
			}
			if (!fixedBool[v2]) {
				moveVecPm[v2] += 1 * k * (currentLengthPm[*ei] - restLengthPm[*ei]) *
					normalize(position[v1] - position[v2]);
			}
		}
	}

	for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {

		//position[*vi] += moveVecPm[*vi];
		position[*vi] += eps * moveVecPm[*vi];
		moveVecPm[*vi] = vec3(0, 0, 0);
	}
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, *g);
		v2 = boost::target(*ei, *g);
		currentLengthPm[*ei] = distance(position[v1], position[v2]);
	}
}
void drawPoints(Graph* g, mat4 proj, vec4 viewp, bool drawNumbers = false, bool drawVertexPoints = false, bool drawCycleList= true) {
	
	if (drawNumbers) {
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);
			vec3 danchorp = position[boost::source(*ei, *g)] + (position[boost::target(*ei, *g)] - position[boost::source(*ei, *g)]) * 0.5f;
			vec2 anchorp1 = glm::project(danchorp, mat4(), proj, viewp);
			gl::drawString(to_string(currentLengthPm[*ei] / restLengthPm[*ei]), anchorp1);
		}
	}
	if (drawVertexPoints)
	{
		for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {

			gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
			vec2 anchorp1 = glm::project(position[*vi], mat4(), proj, viewp);
			gl::drawSolidCircle(anchorp1, 3);
			//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
		}
	}

	if (drawCycleList)
	{
		for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {

			gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
			vec2 anchorp1 = glm::project(position[*vi], mat4(), proj, viewp);
			
			gl::drawString(getCyclesString(cyclesPm[*vi]), anchorp1);

			//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
		}
	}
}

void drawGraph(Graph *g, mat4 proj, vec4 viewp) {
	gl::ScopedColor color(Color::gray(0.2f));
	gl::color(1.0f, 1.0f, 1.0f, 0.8f);
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);
	}
}


#endif // GRAPHSETUP
