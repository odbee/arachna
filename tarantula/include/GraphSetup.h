#ifndef GRAPHSETUP_H_INCLUDED
#define GRAPHSETUP_H_INCLUDED

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"
#include "cinder/Log.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"
#include "cinder/gl/Texture.h"

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
typedef boost::graph_traits<Graph>::edge_iterator edge_ti;

string stringfromVec(vector<size_t> vec) {
	string result = "[ ";
	for (auto it = vec.begin(); it != vec.end(); it++) {
		result.append(to_string(*it));
		result.append(" ");
	}
	result.append(" ]");
	return result;
}
string stringfromCycles(std::vector<std::vector<size_t>> cycles) {
	string result = "[ \n";
	
	for (const auto& cycle : cycles) {
		result.append(stringfromVec(cycle) + " \n");

	}
	result.append(" ]");
	return result;
}

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

std::pair<edge_ti, int> getRandomEdge(Graph* g) {
	tie(ei, eiend) = boost::edges(*g);
	int iteratorLength = 0;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei)
		iteratorLength++;
	auto ei_startEdge = boost::edges(*g).first;
	auto randiter = rand() % iteratorLength;
	for (size_t i = 0; i < randiter; i++)
	{
		ei_startEdge++;
	}
	return make_pair(ei_startEdge, randiter);
}

void splitCyclesAtIndexAndPushBacksecondHalf(std::vector<std::vector<size_t>>* cycles) {

}

void hello() {
	console() << hello;
}
void addRandomCyclicEdge(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs, string& text) {
	tie(ei, eiend) = boost::edges(*g);
	int iteratorLength = 0;

	auto vd_insert1 = boost::add_vertex(*g);
	fixedBool[vd_insert1] = false;
	auto vd_insert2 = boost::add_vertex(*g);
	fixedBool[vd_insert2] = false;
	//created 2 points
	edge_ti ei_startEdge;
	int randiter;
	tie(ei_startEdge, randiter) = getRandomEdge(g);
	//gotten iterator from random iterator 1

	vector<size_t> commonCyclesStartEdge= compareVectors(cyclesPm[source(*ei_startEdge, *g)], cyclesPm[target(*ei_startEdge, *g)]);
	
	auto p_edgeVertex1 = position[source(*ei_startEdge, *g)];
	auto p_edgeVertex2 = position[target(*ei_startEdge, *g)];
	size_t i_start1VertexIndex= source(*ei_startEdge, *g);
	size_t i_start2VertexIndex = target(*ei_startEdge, *g);

	position[vd_insert1] = p_edgeVertex1 + (p_edgeVertex2 - p_edgeVertex1) * float(float(rand() % 1000) / 1000);
	// taken positions of edge points from iterator edge
	int seconditer;
	edge_t ed_goalEdge;

	if (commonCyclesStartEdge.size()) {

		//zwischen hier
		int whichEdgeCycle = rand() % (commonCyclesStartEdge.size());
		size_t cycleIndex = commonCyclesStartEdge[whichEdgeCycle];
		vector<size_t> currentcycle = cycs->at(cycleIndex);

		auto i_edge1indexInCycle = std::find(currentcycle.begin(), currentcycle.end(), i_start1VertexIndex);
		auto i_edge2indexInCycle = std::find(currentcycle.begin(), currentcycle.end(), i_start2VertexIndex);
		auto cyclesize = currentcycle.size();

		if (i_edge1indexInCycle + 1 == i_edge2indexInCycle)       // if clockwise shift second vertex to first index
			rotate(currentcycle.begin(), i_edge2indexInCycle, currentcycle.end());
		else if (i_edge2indexInCycle + 1 == i_edge1indexInCycle) // if counterclockwise shift first vertex to first index
			rotate(currentcycle.begin(), i_edge1indexInCycle, currentcycle.end());

		int randomIndexinCycle = (rand() % (currentcycle.size() - 2)) + 1;

		size_t i_goal1VertexIndex = currentcycle[randomIndexinCycle];
		size_t i_goal2VertexIndex = currentcycle[randomIndexinCycle + 1];
		// get vetex indices
		vector<size_t> commonCyclesGoalEdge = compareVectors(cyclesPm[i_goal1VertexIndex], cyclesPm[i_goal1VertexIndex]);

		vector<size_t> left(currentcycle.begin(), currentcycle.begin() + randomIndexinCycle+1);
		vector<size_t> right(currentcycle.begin() + randomIndexinCycle+1, currentcycle.end());
		


		CI_LOG_I(stringfromVec(left));
		CI_LOG_I(stringfromVec(right));
		

		left.insert(left.end(), { vd_insert2, vd_insert1 });
		right.insert(right.begin(),{ vd_insert1, vd_insert2});
		cycs->at(cycleIndex) = left;
		cycs->push_back(right);
		size_t lastindex = cycs->size() - 1;


	/*	std::replace(cyclesPm[i_start1VertexIndex].begin(), cyclesPm[i_start1VertexIndex].end(),cycleIndex,lastindex);
		std::replace(cyclesPm[randomIndexinCycle].begin(), cyclesPm[randomIndexinCycle].end(), cycleIndex, lastindex);
		*/
		for (const auto& elem : right) {
			std::replace(cyclesPm[elem].begin(), cyclesPm[elem].end(), cycleIndex, lastindex);
		}
		cyclesPm[vd_insert1].insert(cyclesPm[vd_insert1].end(), { cycleIndex, lastindex });
		cyclesPm[vd_insert2].insert(cyclesPm[vd_insert2].end(), { cycleIndex, lastindex });

		ed_goalEdge = my_find_edge(i_goal1VertexIndex, i_goal2VertexIndex, *g);
		// TODO ADD INTERSECTION VERTEX TO ALL INTERSECTED CYCLES
		for (const auto& cycle : commonCyclesGoalEdge) {
			if (cycle!= cycleIndex)
			{
				auto cyc = cycs->at(cycle);
				auto i_edge1indexInCycle = std::find(cyc.begin(), cyc.end(), i_goal1VertexIndex);
				auto i_edge2indexInCycle = std::find(cyc.begin(), cyc.end(), i_goal2VertexIndex);
				if (i_edge1indexInCycle + 1 == i_edge2indexInCycle)       // if clockwise shift second vertex to first index
					cyc.insert(i_edge1indexInCycle+1, vd_insert2);
				else if (i_edge2indexInCycle + 1 == i_edge1indexInCycle) // if counterclockwise shift first vertex to first index
					cyc.insert(i_edge2indexInCycle+1, vd_insert2);
				cycs->at(cycle) = cyc;
				cyclesPm[vd_insert2].push_back(cycle);


			}
		}

		for (const auto& cycle : commonCyclesStartEdge) {
			if (cycle != cycleIndex)
			{

				auto cyc = cycs->at(cycle);
				auto i_edge1indexInCycle = std::find(cyc.begin(), cyc.end(), i_start1VertexIndex);
				auto i_edge2indexInCycle = std::find(cyc.begin(), cyc.end(), i_start2VertexIndex);
				if (i_edge1indexInCycle + 1 == i_edge2indexInCycle) {       // if clockwise shift second vertex to first index
					cyc.insert(i_edge1indexInCycle + 1, vd_insert1);
	
				}
				else if (i_edge2indexInCycle + 1 == i_edge1indexInCycle) { // if counterclockwise shift first vertex to first index
					cyc.insert(i_edge2indexInCycle + 1, vd_insert1);
	
				}
				cycs->at(cycle) = cyc;
				cyclesPm[vd_insert1].push_back(cycle);


			}
		}



	}
	else {
		tie(ei, eiend) = boost::edges(*g);
		iteratorLength = 0;
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			iteratorLength++;
		}
		seconditer = rand() % iteratorLength;
		position[vd_insert1] = p_edgeVertex1 + (p_edgeVertex2 - p_edgeVertex1) * float(float(rand() % 1000) / 1000);
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
		ed_goalEdge = *ei;
	}


	udgcd::printPaths(console(), *cycs);
	//gotten iterator from random iterator 2 
	p_edgeVertex1 = position[source(ed_goalEdge, *g)];
	p_edgeVertex2 = position[target(ed_goalEdge, *g)];
	position[vd_insert2] = p_edgeVertex1 + (p_edgeVertex2 - p_edgeVertex1) * float(float(rand() % 1000) / 1000);
	//set position of edge point 2 from random point on edge
	connectAB(g, vd_insert1, vd_insert2, rc);
	connectAB(g, vd_insert2, source(ed_goalEdge, *g), rc);
	connectAB(g, vd_insert2, target(ed_goalEdge, *g), rc);
	connectAB(g, vd_insert1, source(*ei_startEdge, *g), rc);
	connectAB(g, vd_insert1, target(*ei_startEdge, *g), rc);
	boost::remove_edge(*ei_startEdge, *g);
	boost::remove_edge(ed_goalEdge, *g);
	text= stringfromCycles(*cycs);
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
			
			gl::drawString(to_string(*vi), anchorp1, Color::white(), Font("Times New Roman", 20));
			vec2 offset = vec2(0.0f, 22.0f);
			gl::drawString(getCyclesString(cyclesPm[*vi]), anchorp1+offset, Color::hex(0xFF0000), Font("Times New Roman", 20));

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
