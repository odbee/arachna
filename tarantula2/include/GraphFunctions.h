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

void updatetext(string in) {


	//gl::draw(mTextTexture);
	////std::this_thread::sleep_for(std::chrono::milliseconds(100));
	ofstream myfile;
	myfile.open("example.txt", std::ios_base::app);
	myfile << in;
	myfile.close();
}


void addRandomCyclicEdge(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs) {
	updatetext(to_string(counter) + "v");
	cyclicedge startedge, goaledge;
	edge_ti ei_startEdge;
	int randiter;


	tie(ei_startEdge, randiter) = getRandomEdge(g);
	updatetext(to_string(counter) + "w");
	// initialize startedge:
	{
		startedge.descriptor = *ei_startEdge;
		startedge.cycles = compareVectorsReturnIntersection(cyclesPm[source(startedge.descriptor, *g)], cyclesPm[boost::target(startedge.descriptor, *g)]);
		startedge.start = { source(startedge.descriptor, *g), position[source(startedge.descriptor, *g)] };
		startedge.end = { boost::target(startedge.descriptor, *g), position[boost::target(startedge.descriptor, *g)] };
	}


	updatetext( to_string(counter) + "y");
	counter++;
	int seconditer;
	edge_t ed_goalEdge;
	if (startedge.cycles.size()) {
		updatetext( to_string(counter) + "a");
		size_t cycleIndex = startedge.cycles[rand() % (startedge.cycles.size())];
		// get random cycle index
		vector<size_t> currCyc = cycs->at(cycleIndex);
		auto cyclesize = currCyc.size();
		auto edge1indInCycle_i = std::find(currCyc.begin(), currCyc.end(), startedge.start.index);
		auto edge2indInCycle_i = std::find(currCyc.begin(), currCyc.end(), startedge.end.index);
		{updatetext(to_string(counter) + "b");
		updatetext(stringfromVec(currCyc) + "( " +
			to_string(std::distance(currCyc.begin(), edge1indInCycle_i)) + " , " +
			to_string(std::distance(currCyc.begin(), edge2indInCycle_i)) + " )");
		}

		if ((std::distance(currCyc.begin(), edge1indInCycle_i) +1 )% cyclesize == std::distance(currCyc.begin(), edge2indInCycle_i))       // if clockwise shift second vertex to first index
			rotate(currCyc.begin(), edge2indInCycle_i, currCyc.end());
		else if ((std::distance(currCyc.begin(), edge2indInCycle_i) + 1) % cyclesize == std::distance(currCyc.begin(), edge1indInCycle_i)) // if counterclockwise shift first vertex to first index
			rotate(currCyc.begin(), edge1indInCycle_i, currCyc.end());
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
			
			if (auto retedge = GetEdgeFromItsVerts(goaledge.start.index, goaledge.end.index, *g)) {
				ed_goalEdge = *retedge;
			}
			else {
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
		left.insert(left.end(), { goaledge.divisionvert, startedge.divisionvert });
		right.insert(right.begin(), { startedge.divisionvert, goaledge.divisionvert });
		cycs->at(cycleIndex) = left;
		cycs->push_back(right);
		size_t lastindex = cycs->size() - 1;
		updatetext( to_string(counter) + "f");
		auto commoncycles = compareVectorsReturnIntersection(goaledge.cycles, startedge.cycles);
		console() << "common cycles of goal and start edge: " << stringfromCyclesShort(commoncycles) << endl;
		for (const auto& elem : right) {
			std::replace(cyclesPm[elem].begin(), cyclesPm[elem].end(), cycleIndex, lastindex);
			//console() << "cycles before: " << stringfromCyclesShort(cyclesPm[elem]) << endl;
			//
			//if (elem != startedge.divisionvert && elem != goaledge.divisionvert) {
			//	for (const size_t& cycleN : commoncycles) {
			//		vector<size_t> curcur = cycs->at(cycleN);
			//		cyclesPm[elem].erase(std::remove(cyclesPm[elem].begin(), cyclesPm[elem].end(), cc), cyclesPm[elem].end());
			//		curcur.erase(std::remove(curcur.begin(), curcur.end(), elem), curcur.end());
			//		cycs->at(cycleN) = curcur;
			//	}
			//}
			//
			//console() << "cycles after: " << stringfromCyclesShort(cyclesPm[elem]) << endl;

		}


		cyclesPm[startedge.divisionvert].insert(cyclesPm[startedge.divisionvert].end(), { cycleIndex, lastindex });
		cyclesPm[goaledge.divisionvert].insert(cyclesPm[goaledge.divisionvert].end(), { cycleIndex, lastindex });
		updatetext( to_string(counter) + "g");
		// TODO ADD INTERSECTION VERTEX TO ALL INTERSECTED CYCLES
		updatetext( to_string(counter) + "é");

		addIntersectionVertexToCycles(goaledge, cycleIndex, cycs);
		addIntersectionVertexToCycles(startedge, cycleIndex, cycs);
		updatetext( to_string(counter) + "h");
	}
	else {
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

	updatetext( to_string(counter) + "i");
	//udgcd::printPaths(console(), *cycs);
	//gotten iterator from random iterator 2 
	goaledge.start = { source(ed_goalEdge, *g), position[source(ed_goalEdge, *g)] };
	goaledge.end = { boost::target(ed_goalEdge, *g), position[boost::target(ed_goalEdge, *g)] };

	position[goaledge.divisionvert] = interpolate(goaledge, float(float(rand() % 1000) / 1000));
	updatetext( to_string(counter) + "j");
	connectAB(g, startedge.divisionvert, goaledge.divisionvert, rc);

	connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[ed_goalEdge]);
	connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[ed_goalEdge]);
	
	connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor]);
	connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor]);
	updatetext( to_string(counter) + "k");
	boost::remove_edge(startedge.descriptor, *g);
	boost::remove_edge(ed_goalEdge, *g);
	//text = stringfromCycles(*cycs);
	updatetext( to_string(counter) + "l");
}
