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

void connectEdges(Graph* g, cyclicedge startedge, cyclicedge goaledge, float rc) {
	connectAB(g, startedge.divisionvert, goaledge.divisionvert, rc);
	if (forbiddenPm[goaledge.descriptor]) {
		connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor], true);
		connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor], true);
		fixedBool[goaledge.divisionvert] = true;
	}
	else {
		connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor]);
		connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor]);
	}

	if (forbiddenPm[startedge.descriptor]) {
		connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor], true);
		connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor], true);
		fixedBool[startedge.divisionvert] = true;
	}
	else {
		connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor]);
		connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor]);
	}
	boost::remove_edge(startedge.descriptor, *g);
	boost::remove_edge(goaledge.descriptor, *g);
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


void initEdge(edge_t ed, cyclicedge& cedge, Graph g)
{
	cedge.descriptor = ed;
	cedge.cycles = compareVectorsReturnIntersection(cyclesPm[source(cedge.descriptor, g)], cyclesPm[boost::target(cedge.descriptor, g)]);
	cedge.start = { source(cedge.descriptor, g), position[source(cedge.descriptor, g)] };
	cedge.end = { boost::target(cedge.descriptor, g), position[boost::target(cedge.descriptor, g)] };
}


void updatetext(string in) {


	//gl::draw(mTextTexture);
	////std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//ofstream myfile;
	//myfile.open("example.txt", std::ios_base::app);
	//myfile << in;
	//myfile.close();
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
	cyclicedge startedge, goaledge; edge_ti ei_startEdge;
	ei_startEdge = getRandomEdgeWeightedByLength(g);
	initEdge(*ei_startEdge, startedge,*g);
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
			isforbidden = forbiddenPm[ed_goalEdge] && forbiddenPm[startedge.descriptor] && CHECKFORBIDDEN;
			
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
		position[startedge.divisionvert] = interpolate(startedge, 0.5f);
		//position[startedge.divisionvert] = interpolate(startedge, float((float(rand() % 5) + 1) / 6));
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

	position[goaledge.divisionvert] = interpolate(goaledge, 0.5f);
	//position[goaledge.divisionvert] = interpolate(goaledge, float((float(rand() % 5) + 1) / 6));

	updatetext( to_string(counter) + "j");
	connectAB(g, startedge.divisionvert, goaledge.divisionvert, rc);

	if (forbiddenPm[goaledge.descriptor]) {
		connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor],true);
		connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor],true);
		fixedBool[goaledge.divisionvert] = true;
	}
	else {
		connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor]);
		connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor]);
	}
	
	if (forbiddenPm[startedge.descriptor]) {
		connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor],true);
		connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor],true);
		fixedBool[startedge.divisionvert] = true;
	}
	else {
		connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor]);
		connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor]);
	}
	updatetext( to_string(counter) + "k");
	boost::remove_edge(startedge.descriptor, *g);
	boost::remove_edge(goaledge.descriptor, *g);
	//text = stringfromCycles(*cycs);
	updatetext( to_string(counter) + "l");
}


void addRandomCyclicEdgeAvoidArea(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs) {
	cyclicedge startedge, goaledge;	edge_ti ei_startEdge; int randiter;
	ei_startEdge = getRandomEdgeWeightedByTension(g);
	initEdge(*ei_startEdge, startedge, *g);
	{
		displayEdgeV_i = startedge.start.index;
		displayEdgeV_ii = startedge.end.index;
	}

	int seconditer;	edge_t ed_goalEdge;
	if (startedge.cycles.size()) {
		size_t cycleIndex = startedge.cycles[rand() % (startedge.cycles.size())];
		vector<size_t> currCyc = cycs->at(cycleIndex);
		displayCycle_i = cycleIndex;

		shiftCycle(currCyc, startedge.start.index, startedge.end.index);

		updatetext(to_string(counter) + "c");
		int randIndInCyc = 0;
		bool isforbidden = true;
		int seed1 = 0;
		int rint = 0;
		int cc;
		while (isforbidden) {
			rint = rand();
			cc = (currCyc.size());
			randIndInCyc = (rint % (currCyc.size() - 1));

			goaledge.start.index = currCyc[randIndInCyc];
			goaledge.end.index = currCyc[(randIndInCyc + 1) % currCyc.size()];

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
			auto auter= interpolate(startedge, 0.5f);
			auto goaler = interpolate(goaledge, 0.5f);
			
			auto constaxxx = 2.4f;
			if (abs(auter.x) <= constaxxx && abs(auter.y) <= constaxxx && abs(auter.z) <= constaxxx && abs(goaler.x) <= constaxxx && abs(goaler.y) <= constaxxx && abs(goaler.z) <= constaxxx) {
				console() << to_string(auter) << " ,,, " << to_string(goaler) << endl;
				return;
			}
			isforbidden = forbiddenPm[ed_goalEdge] && forbiddenPm[startedge.descriptor] && CHECKFORBIDDEN;

		}
		// get vetex indices
		goaledge.cycles = compareVectorsReturnIntersection(cyclesPm[goaledge.start.index], cyclesPm[goaledge.end.index]);

		updatetext(to_string(counter) + "d");
		vector<size_t> left(currCyc.begin(), currCyc.begin() + randIndInCyc + 1);
		vector<size_t> right(currCyc.begin() + randIndInCyc + 1, currCyc.end());
		updatetext(to_string(counter) + "e");

		startedge.divisionvert = boost::add_vertex(*g); fixedBool[startedge.divisionvert] = false;
		goaledge.divisionvert = boost::add_vertex(*g); fixedBool[goaledge.divisionvert] = false;
		//created 2 points

		
		position[startedge.divisionvert] = interpolate(startedge, 0.5f);
		position[goaledge.divisionvert] = interpolate(goaledge, 0.5f);

		//position[startedge.divisionvert] = interpolate(startedge, float((float(rand() % 5) + 1) / 6));
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

	updatetext(to_string(counter) + "i");
	//udgcd::printPaths(console(), *cycs);
	//gotten iterator from random iterator 2 


	//position[goaledge.divisionvert] = interpolate(goaledge, float((float(rand() % 5) + 1) / 6));

	updatetext(to_string(counter) + "j");
	connectAB(g, startedge.divisionvert, goaledge.divisionvert, rc);

	if (forbiddenPm[goaledge.descriptor]) {
		connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor], true);
		connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor], true);
		fixedBool[goaledge.divisionvert] = true;
	}
	else {
		connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor]);
		connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor]);
	}

	if (forbiddenPm[startedge.descriptor]) {
		connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor], true);
		connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor], true);
		fixedBool[startedge.divisionvert] = true;
	}
	else {
		connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor]);
		connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor]);
	}
	updatetext(to_string(counter) + "k");
	boost::remove_edge(startedge.descriptor, *g);
	boost::remove_edge(goaledge.descriptor, *g);
	//text = stringfromCycles(*cycs);
	updatetext(to_string(counter) + "l");
}


std::vector<edge_t> getConnectedEdges(Graph* g, cyclicedge edge, std::vector<std::vector<size_t>>* cycs, std::vector<size_t>& edgeindices) {
	edge_t hs;
	std::vector<size_t> availverts;
	
	std::vector<edge_t> availedges;
	
	edge_t cachedge;
	int countr = 0;
	for ( std::vector<size_t>& cycle : cycles) {
		std::vector<size_t>::iterator it1= std::find(cycle.begin(), cycle.end(), edge.end.index);
		std::vector<size_t>::iterator it2 = std::find(cycle.begin(), cycle.end(), edge.start.index);
		if (it1 != cycle.end() && it2 != cycle.end()) {
			for (size_t i = 0; i < cycle.size(); i++)
			{
				// only add vertex if inside
				if (auto retedge = GetEdgeFromItsVerts(cycle[i], cycle[(i + 1) % cycle.size()], *g)) {
					edge_t cachedge = *retedge;
					if (std::find(availedges.begin(), availedges.end(), cachedge) == availedges.end()) {
						// someName not in name, add it

						if (!forbiddenPm[cachedge] && cachedge!=edge.descriptor) { //FORBIDDENCHECK ADDED
							availedges.push_back(cachedge);
							edgeindices.push_back(countr);
						}
						
					}
				}

				
			}
		}
		countr++;
	}
	// TODO make clause for no edges, returning no edg

	return availedges;
}
void addRandomCyclicEdgeTesting(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs) {
	cyclicedge startedge, goaledge;	edge_ti ei_startEdge;
	ei_startEdge = getRandomEdgeWeightedByX(g);
	edge_t ed_startEdge = getRandomEdgeFromEdgeListT(g, boost::edges(*g).first, boost::edges(*g).second,true);
	initEdge(ed_startEdge, startedge, *g);
	{
		displayEdgeV_i = startedge.start.index;
		displayEdgeV_ii = startedge.end.index;
	}
	int seconditer;	edge_t ed_goalEdge; edge_ti ei_goalEdge;
	if (startedge.cycles.size()) {
		std::vector<size_t>edgeinds;
		auto connedges = getConnectedEdges(g, startedge, cycs, edgeinds);
		ed_goalEdge = getRandomEdgeFromEdgeListT(g, connedges.begin(),connedges.end());
		size_t cycleIndex = *(edgeinds.begin() + std::distance(connedges.begin(), std::find(connedges.begin(), connedges.end(), ed_goalEdge)));

		initEdge(ed_goalEdge, goaledge, *g);
		updatetext(to_string(counter) + "a");
		displayEdgeV_iii = goaledge.start.index;
		displayEdgeV_iv = goaledge.end.index;
		updatetext(to_string(counter) + "b");
		/*size_t cycleIndex = goaledge.cycles[0];*/
		vector<size_t> currCyc = cycs->at(cycleIndex);
		updatetext(to_string(counter) + "c");
		displayCycle_i = cycleIndex;
		shiftCycle(currCyc, startedge.start.index, startedge.end.index);

		auto find1 = std::find(currCyc.begin(), currCyc.end(), goaledge.start.index);
		auto find2 = std::find(currCyc.begin(), currCyc.end(), goaledge.end.index);
		auto gei1= std::distance(currCyc.begin(),find1);
		auto gei2= std::distance(currCyc.begin(), find2);
		auto gei = (gei1 < gei2) ? find2 : find1;
		// get vetex indices
		updatetext(to_string(counter) + "d" + " ("+ "values:" + to_string(goaledge.start.index) + ", " + to_string(goaledge.end.index) + ", " "indices"  + to_string(gei1) + "," + to_string(gei2) + +";" + stringfromCyclesShort(currCyc) + ") ");


		vector<size_t> left(currCyc.begin(), gei);
		vector<size_t> right(gei, currCyc.end());
		updatetext(to_string(counter) + " (" + "left:" + stringfromCyclesShort(left) + ", right:" + stringfromCyclesShort(right) + ") ");
		updatetext(to_string(counter) + "e");
		startedge.divisionvert = boost::add_vertex(*g); fixedBool[startedge.divisionvert] = false;
		goaledge.divisionvert = boost::add_vertex(*g); fixedBool[goaledge.divisionvert] = false;
		//created 2 points
		position[startedge.divisionvert] = interpolate(startedge, 0.5f);
		position[goaledge.divisionvert] = interpolate(goaledge, 0.5f);
		//position[startedge.divisionvert] = interpolate(startedge, float((float(rand() % 5) + 1) / 6));
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
	updatetext(to_string(counter) + "i");
	connectEdges(g, startedge, goaledge, rc);
	updatetext(to_string(counter) + "j");
	counter++;
	updatetext("\n");
}