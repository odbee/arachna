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

void updatetext(string& text, string in) {
	text = in;
	//render();
	//gl::draw(mTextTexture);
	////std::this_thread::sleep_for(std::chrono::milliseconds(100));
	ofstream myfile;
	myfile.open("example.txt", std::ios_base::app);
	myfile << in;
	myfile.close();
}


void addRandomCyclicEdge(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs, string& text) {
	updatetext(text, to_string(counter) + "v");


	cyclicedge startedge, goaledge;
	startedge.divisionvert= boost::add_vertex(*g); fixedBool[startedge.divisionvert] = false;
	goaledge.divisionvert = boost::add_vertex(*g); fixedBool[goaledge.divisionvert] = false;
	//created 2 points

	edge_ti ei_startEdge;
	int randiter;
	updatetext(text, to_string(counter) + "w");



	tie(ei_startEdge, randiter) = getRandomEdge(g);
	startedge.descriptor = *ei_startEdge;
	//gotten iterator from random iterator 1
	startedge.cycles = compareVectorsReturnIntersection(cyclesPm[source(startedge.descriptor, *g)], cyclesPm[boost::target(startedge.descriptor, *g)]);
	// gotten list of common cycles of start edge


	startedge.start = { source(startedge.descriptor, *g), position[source(startedge.descriptor, *g)] };
	startedge.end = { boost::target(startedge.descriptor, *g), position[boost::target(startedge.descriptor, *g)] };

	updatetext(text, to_string(counter) + "x");
	auto p_edgeVertex1 = position[source(startedge.descriptor, *g)];
	auto p_edgeVertex2 = position[boost::target(startedge.descriptor, *g)];
	size_t start1VertInd_i = source(startedge.descriptor, *g);
	size_t start2VertInd_i = boost::target(startedge.descriptor, *g);
	position[startedge.divisionvert] =interpolate(startedge, float(float(rand() % 1000) / 1000));
	// taken positions of edge points from iterator edge
	updatetext(text, to_string(counter) + "y");
	counter++;
	int seconditer;
	edge_t ed_goalEdge;
	if (startedge.cycles.size()) {
		updatetext(text, to_string(counter) + "a");

		size_t cycleIndex = startedge.cycles[rand() % (startedge.cycles.size())];
		// get random cycle index
		vector<size_t> currCyc = cycs->at(cycleIndex);
		auto cyclesize = currCyc.size();
		auto edge1indInCycle_i = std::find(currCyc.begin(), currCyc.end(), start1VertInd_i);
		auto edge2indInCycle_i = std::find(currCyc.begin(), currCyc.end(), start2VertInd_i);
		updatetext(text, to_string(counter) + "b");
		updatetext(text, stringfromVec(currCyc) + "( " +
			to_string(std::distance(currCyc.begin(), edge1indInCycle_i)) + " , " +
			to_string(std::distance(currCyc.begin(), edge2indInCycle_i)) +  " )");


		if ((std::distance(currCyc.begin(), edge1indInCycle_i) +1 )% cyclesize == std::distance(currCyc.begin(), edge2indInCycle_i))       // if clockwise shift second vertex to first index
			rotate(currCyc.begin(), edge2indInCycle_i, currCyc.end());
		else if ((std::distance(currCyc.begin(), edge2indInCycle_i) + 1) % cyclesize == std::distance(currCyc.begin(), edge1indInCycle_i)) // if counterclockwise shift first vertex to first index
			rotate(currCyc.begin(), edge1indInCycle_i, currCyc.end());
		updatetext(text, to_string(counter) + "c");
		int randIndInCyc = (rand() % (currCyc.size() - 2)) + 1;
		goaledge.start.index = currCyc[randIndInCyc];
		goaledge.end.index = currCyc[randIndInCyc + 1];
		// get vetex indices
		goaledge.cycles = compareVectorsReturnIntersection(cyclesPm[goaledge.start.index], cyclesPm[goaledge.end.index]);
		updatetext(text, to_string(counter) + "d");
		vector<size_t> left(currCyc.begin(), currCyc.begin() + randIndInCyc + 1);
		vector<size_t> right(currCyc.begin() + randIndInCyc + 1, currCyc.end());
		updatetext(text, to_string(counter) + "e");
		left.insert(left.end(), { goaledge.divisionvert, startedge.divisionvert });
		right.insert(right.begin(), { startedge.divisionvert, goaledge.divisionvert });
		cycs->at(cycleIndex) = left;
		cycs->push_back(right);
		size_t lastindex = cycs->size() - 1;
		updatetext(text, to_string(counter) + "f");
		for (const auto& elem : right) {
			std::replace(cyclesPm[elem].begin(), cyclesPm[elem].end(), cycleIndex, lastindex);
		}
		cyclesPm[startedge.divisionvert].insert(cyclesPm[startedge.divisionvert].end(), { cycleIndex, lastindex });
		cyclesPm[goaledge.divisionvert].insert(cyclesPm[goaledge.divisionvert].end(), { cycleIndex, lastindex });
		updatetext(text, to_string(counter) + "g");
		ed_goalEdge = GetEdgeFromItsVerts(goaledge.start.index, goaledge.end.index, *g);
		// TODO ADD INTERSECTION VERTEX TO ALL INTERSECTED CYCLES
		updatetext(text, to_string(counter) + "é");

		addIntersectionVertexToCycles(goaledge, cycleIndex, cycs);
		addIntersectionVertexToCycles(startedge, cycleIndex, cycs);
		updatetext(text, to_string(counter) + "h");
	}
	else {
		text = "nocyc";
		tie(ei, eiend) = boost::edges(*g);
		int iteratorLength = 0;
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			iteratorLength++;
		}
		seconditer = rand() % iteratorLength;

		position[startedge.divisionvert] = interpolate(startedge, float(float(rand() % 1000) / 1000));
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
		goaledge.descriptor = *ei;
	}

	updatetext(text, to_string(counter) + "i");
	//udgcd::printPaths(console(), *cycs);
	//gotten iterator from random iterator 2 
	goaledge.start = { source(ed_goalEdge, *g), position[source(ed_goalEdge, *g)] };
	goaledge.end = { boost::target(ed_goalEdge, *g), position[boost::target(ed_goalEdge, *g)] };

	position[goaledge.divisionvert] = interpolate(goaledge, float(float(rand() % 1000) / 1000));
	updatetext(text, to_string(counter) + "j");
	connectAB(g, startedge.divisionvert, goaledge.divisionvert, rc);
	connectAB(g, goaledge.divisionvert, goaledge.start.index, rc);
	connectAB(g, goaledge.divisionvert, goaledge.end.index, rc);
	connectAB(g, startedge.divisionvert, startedge.start.index, rc);
	connectAB(g, startedge.divisionvert, startedge.end.index, rc);
	updatetext(text, to_string(counter) + "k");
	boost::remove_edge(startedge.descriptor, *g);
	boost::remove_edge(ed_goalEdge, *g);
	//text = stringfromCycles(*cycs);
	updatetext(text, to_string(counter) + "l");
}
