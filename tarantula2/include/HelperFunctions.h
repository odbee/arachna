#pragma once
#include "GraphSetup.h"


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

string stringfromCyclesShort(std::vector<size_t> cycleslist) {
	string outtext;
	for (const auto& elem : cycleslist) {
		outtext.append(to_string(elem) + "  ");

	}
	return outtext;
}



void connectAB(Graph* g, Graph::vertex_descriptor endPointA, Graph::vertex_descriptor endPointB, float rc) {
	float dd = distance(position[endPointA], position[endPointB]);
	Graph::edge_descriptor edge = boost::add_edge(endPointA, endPointB, *g).first;
	currentLengthPm[edge] = dd;
	restLengthPm[edge] = dd * rc;
	return;
}

edge_t GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g)
{
	for (auto e : boost::make_iterator_range(out_edges(v, g))) {
		if (target(e, g) == u)
			return e;
	}
	throw std::domain_error("my_find_edge: not found");
}


void addCyclesToVertices(Graph* g, std::vector<std::vector<size_t>> cycles) {
	for (int i = 0; i < cycles.size(); i++)
	{
		for (const auto& elem : cycles[i])
			cyclesPm[elem].push_back(i);
	}
}

void emptyCyclesFromVertices(Graph* g) {
	for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {
		cyclesPm[*vi].clear();
	}
}


//https://stackoverflow.com/questions/5225820/compare-two-vectors-c
// rethink usage of const vector<size_t>
vector<size_t> compareVectorsReturnIntersection(const vector<size_t> vec1, const vector<size_t> vec2) {
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

void setInitialWeb(Graph* g, float rc) {
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
	connectAB(g, a, b, rc);
	connectAB(g, a, c, rc);
	connectAB(g, a, e, rc);




	connectAB(g, b, d, rc);
	connectAB(g, b, c, rc);
	connectAB(g, b, e, rc);

	connectAB(g, c, d, rc);
	connectAB(g, c, e, rc);

	connectAB(g, e, d, rc);
	
	cycles.push_back({ a,b,d });
	cycles.push_back({ a,c,d });
	cycles.push_back({ a,e,d });
	cycles.push_back({ b,c,d });
	cycles.push_back({ b,e,d });
	cycles.push_back({ c,e,d });
	addCyclesToVertices(g, cycles);



}

vec3 interpolateBetweenPoints(vec3 point1, vec3 point2, float t) {
	return point1 + (point2 - point1) * t;
}