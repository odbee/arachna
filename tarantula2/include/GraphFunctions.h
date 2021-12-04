#pragma once
#include "GraphSetup.h"
#include "HelperFunctions.h"
#include "TextHelper.h"
#include <chrono>
#include <thread>




struct cyclicvert {
	size_t index;
	vec3 pos;

};

struct cyclicedge {
	edge_ti ei_iterator;
	edge_t descriptor;
	cyclicvert start;
	cyclicvert end;
	vector<size_t> cycles;
	vertex_t divisionvert;
};

int counter = 0;
vec3 interpolate(cyclicedge edge, float t) {
	return edge.start.pos + (edge.end.pos - edge.start.pos) * t;
}

void removebyindex(vector<size_t>& vec, size_t ind) {
	vec.erase(std::remove(vec.begin(), vec.end(), ind), vec.end());
}
void addIntersectionVertexToCycles(cyclicedge& edge, int cycleIndex, std::vector<std::vector<size_t>>* cycs) {
	for (const auto& cycle : edge.cycles) {
		if (cycle != cycleIndex)
		{
			auto cyc = cycs->at(cycle);
			auto edge1indInCycle_i = std::find(cyc.begin(), cyc.end(), edge.start.index);
			auto edge2indInCycle_i = std::find(cyc.begin(), cyc.end(), edge.end.index);
			if ((std::distance(cyc.begin(), edge1indInCycle_i) + 1) % cyc.size() == std::distance(cyc.begin(), edge2indInCycle_i))       // if clockwise shift second vertex to first index
				cyc.insert(edge1indInCycle_i + 1, edge.divisionvert);
			else if ((std::distance(cyc.begin(), edge2indInCycle_i) + 1) % cyc.size() == std::distance(cyc.begin(), edge1indInCycle_i))       // if clockwise shift second vertex to first  // if counterclockwise shift first vertex to first index
				cyc.insert(edge2indInCycle_i + 1, edge.divisionvert);
			cycs->at(cycle) = cyc;
			cyclesPm[edge.divisionvert].push_back(cycle);
		}
	}
}

void shiftCycle(std::vector<size_t>& cycle, int index1,int  index2) {
	auto edge1indInCycle_i = std::find(cycle.begin(), cycle.end(), index1);
	auto edge2indInCycle_i = std::find(cycle.begin(), cycle.end(), index2);
	auto cyclesize = cycle.size();
	if ((std::distance(cycle.begin(), edge1indInCycle_i) + 1) % cyclesize == std::distance(cycle.begin(), edge2indInCycle_i))       // if clockwise shift second vertex to first index
		rotate(cycle.begin(), edge2indInCycle_i, cycle.end());
	else if ((std::distance(cycle.begin(), edge2indInCycle_i) + 1) % cyclesize == std::distance(cycle.begin(), edge1indInCycle_i)) { // if counterclockwise flip list and do as in clockwise
		std::reverse(cycle.begin(), cycle.end());
		edge1indInCycle_i = std::find(cycle.begin(), cycle.end(), index1);
		edge2indInCycle_i = std::find(cycle.begin(), cycle.end(), index2);
		rotate(cycle.begin(), edge2indInCycle_i, cycle.end());
	}
	else {
		console() << "vertex 1 and 2 are not adjacent" << endl;
	}
}


void initEdge(edge_ti ei, cyclicedge& cedge, Graph g)
{
	cedge.descriptor = *ei;
	cedge.cycles = compareVectorsReturnIntersection(cyclesPm[source(cedge.descriptor, g)], cyclesPm[boost::target(cedge.descriptor, g)]);
	cedge.start = { source(cedge.descriptor, g), position[source(cedge.descriptor, g)] };
	cedge.end = { boost::target(cedge.descriptor, g), position[boost::target(cedge.descriptor, g)] };
}


void updatetext(string in) {


	//gl::draw(mTextTexture);
	////std::this_thread::sleep_for(std::chrono::milliseconds(100));
	ofstream myfile;
	myfile.open("example.txt", std::ios_base::app);
	myfile << in;
	myfile.close();
}

std::vector<size_t> vectorcycles(std::vector<size_t> vec) {
	std::vector<size_t> result = cyclesPm[vec[0]];
	for (size_t i = 1; i < vec.size(); i++) {
		result= compareVectorsReturnIntersection(result, cyclesPm[vec[i]]);
	}
	return result;
}

void resolveIntersections(cyclicedge start,cyclicedge goal,size_t cycle1index, size_t cycle2index,Graph& g, std::vector<std::vector<size_t>>& cycs) {
	auto& left = cycs[cycle1index];
	auto& right = cycs[cycle2index];
	left.insert(left.end(), { goal.divisionvert, start.divisionvert });
	right.insert(right.begin(), { start.divisionvert, goal.divisionvert });
	for (const auto& elem : right)
		std::replace(cyclesPm[elem].begin(), cyclesPm[elem].end(), cycle1index, cycle2index);
	addIntersectionVertexToCycles(goal, cycle1index, &cycs);
	addIntersectionVertexToCycles(start, cycle1index, &cycs);
	auto commoncycles = compareVectorsReturnIntersection(goal.cycles, start.cycles);
	auto cright=vectorcycles(right);
	auto cleft = vectorcycles(left);


	for (const auto& cycleN : cright)
	{
		for (size_t i = 2; i < right.size(); i++)
		{
			auto elem = right[i];
			removebyindex(cyclesPm[elem], cycleN);
			removebyindex(cycs.at(cycleN), elem);
		}
	}

	for (const auto& cycleN : cleft)
	{
		for (size_t i = 2; i < left.size(); i++)
		{
			auto elem = left[i];
			removebyindex(cyclesPm[elem], cycleN);
			removebyindex(cycs.at(cycleN), elem);
		}
	}

	cyclesPm[start.divisionvert].insert(cyclesPm[start.divisionvert].end(), { cycle1index, cycle2index });
	cyclesPm[goal.divisionvert].insert(cyclesPm[goal.divisionvert].end(), { cycle1index, cycle2index });
}


void addRandomCyclicEdge(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs) {
	updatetext(to_string(counter) + "v");
	cyclicedge startedge, goaledge;
	edge_ti ei_startEdge;
	int randiter;

	tie(ei_startEdge, randiter) = getRandomEdge(g);
	initEdge(ei_startEdge, startedge,*g);
	displayEdgeV_i = startedge.start.index;
	displayEdgeV_ii = startedge.end.index;
	int seconditer;
	edge_t ed_goalEdge;
	if (startedge.cycles.size()) {
		size_t cycleIndex = startedge.cycles[rand() % (startedge.cycles.size())];
		vector<size_t> currCyc = cycs->at(cycleIndex);
		displayCycle_i = cycleIndex;

		shiftCycle(currCyc, startedge.start.index, startedge.end.index);

		updatetext( to_string(counter) + "c");
		int randIndInCyc = 0;
		bool isforbidden = true;
		int seed1=0;
		int rint = 0;
		int cc;
		while (isforbidden) {
			rint = rand();
			cc = (currCyc.size());
			randIndInCyc = (rint % (currCyc.size() - 1));

			goaledge.start.index = currCyc[randIndInCyc];
			goaledge.end.index = currCyc[(randIndInCyc + 1) %currCyc.size()];

			displayEdgeV_iii = goaledge.start.index;
			displayEdgeV_iv = goaledge.end.index;

			if (auto retedge = GetEdgeFromItsVerts(goaledge.start.index, goaledge.end.index, *g)) {
				ed_goalEdge = *retedge;
				goaledge.descriptor = *retedge;
			}
			else {
				console() << "[" << iterationcounter << "]" << "found an invalid edge.skipping" << endl;
				return;
			}
			isforbidden = forbiddenPm[ed_goalEdge] && CHECKFORBIDDEN;
			
		}
		// get vetex indices
		goaledge.cycles = compareVectorsReturnIntersection(cyclesPm[goaledge.start.index], cyclesPm[goaledge.end.index]);

		updatetext( to_string(counter) + "d");
		vector<size_t> left(currCyc.begin(), currCyc.begin() + randIndInCyc + 1);
		vector<size_t> right(currCyc.begin() + randIndInCyc + 1, currCyc.end());
		updatetext( to_string(counter) + "e");

		startedge.divisionvert = boost::add_vertex(*g); fixedBool[startedge.divisionvert] = false;
		goaledge.divisionvert = boost::add_vertex(*g); fixedBool[goaledge.divisionvert] = false;
		//created 2 points

		position[startedge.divisionvert] = interpolate(startedge, float(float(rand() % 1000) / 1000));
		cycs->at(cycleIndex) = left;
		cycs->push_back(right);
		size_t lastindex = cycs->size() - 1;
		displayCycle_ii = lastindex;
		resolveIntersections(startedge, goaledge, cycleIndex, lastindex, *g, *cycs);

	}
	else {
		console() << " no cycles found, returning" << endl; 
		return;
	}

	updatetext( to_string(counter) + "i");
	//udgcd::printPaths(console(), *cycs);
	//gotten iterator from random iterator 2 
	goaledge.start = { source(goaledge.descriptor, *g), position[source(goaledge.descriptor, *g)] };
	goaledge.end = { boost::target(goaledge.descriptor, *g), position[boost::target(goaledge.descriptor, *g)] };

	position[goaledge.divisionvert] = interpolate(goaledge, float(float(rand() % 1000) / 1000));
	updatetext( to_string(counter) + "j");
	connectAB(g, startedge.divisionvert, goaledge.divisionvert, rc);

	connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor]);
	connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor]);
	
	connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor]);
	connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor]);
	updatetext( to_string(counter) + "k");
	boost::remove_edge(startedge.descriptor, *g);
	boost::remove_edge(goaledge.descriptor, *g);
	//text = stringfromCycles(*cycs);
	updatetext( to_string(counter) + "l");
}
