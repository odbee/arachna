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

vector<edge_t> connectEdges(Graph* g, cyclicedge startedge, cyclicedge goaledge, float rc) {
	vector<edge_t> returnedges(5);

	returnedges[0]=connectAB(g, startedge.divisionvert, goaledge.divisionvert, rc);
	
	if (forbiddenPm[goaledge.descriptor]) {
		returnedges[1] = connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor], true);
		returnedges[2] = connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor], true);
		fixedBool[goaledge.divisionvert] = true;
	}
	else {
		returnedges[1] = connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor]);
		returnedges[2] = connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor]);
	}

	if (forbiddenPm[startedge.descriptor]) {
		returnedges[3] = connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor], true);
		returnedges[4] = connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor], true);
		fixedBool[startedge.divisionvert] = true;
	}
	else {
		returnedges[3] = connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor]);
		returnedges[4] = connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor]);
	}
	boost::remove_edge(startedge.descriptor, *g);
	boost::remove_edge(goaledge.descriptor, *g);
	return returnedges;
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


	gl::draw(mTextTexture);
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
	updatetext(to_string(counter) + "z");

	// TODO FIX THE ISSUE WHEN THERE ARE NO AVAILABLE EDEGES
	cyclicedge startedge, goaledge;	edge_ti graphstart, graphend; edge_ti ei_startEdge; edge_t ed_startEdge;
	//getRandomEdgeWeightedByX(g);
	updatetext(to_string(counter) + "y");
	tie(ei, eiend) = boost::edges(*g); //need this, will get error otherwise
	tie(graphstart, graphend) = boost::edges(*g);
	updatetext(to_string(counter) + "x");
	ed_startEdge = getRandomEdgeFromEdgeListIntegrated(g, graphstart, graphend,true);
	updatetext(to_string(counter) + "w");
	initEdge(ed_startEdge, startedge, *g);
	{
		displayEdgeV_i = startedge.start.index;
		displayEdgeV_ii = startedge.end.index;
	}
	updatetext(to_string(counter) + "v");
	std::vector<size_t>edgeinds;
	updatetext(to_string(counter) + "u");

	auto connedges = getConnectedEdges(g, startedge, cycs, edgeinds);

	int seconditer;	edge_t ed_goalEdge; edge_ti ei_goalEdge;
	if (connedges.size()) {
		

		updatetext(to_string(counter) + "t" + "[listlength" + to_string(connedges.size()) + "]");

		ed_goalEdge = getRandomEdgeFromEdgeListIntegrated(g, connedges.begin(),connedges.end());
		updatetext(to_string(counter) + "s");

		size_t cycleIndex = *(edgeinds.begin() + std::distance(connedges.begin(), std::find(connedges.begin(), connedges.end(), ed_goalEdge)));
		updatetext(to_string(counter) + "r");

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
		//getDivPoint(startedge.descriptor);
		position[startedge.divisionvert] = interpolate(startedge, getDivPoint(startedge.descriptor));
		position[goaledge.divisionvert] = interpolate(goaledge, getDivPoint(startedge.descriptor));
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
	console() << GLOBALINT << endl;
	updatetext("\n");
}


void addRandomCyclicEdgeAnimated(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs) {
	// TODO FIX THE ISSUE WHEN THERE ARE NO AVAILABLE EDEGES
	
	cyclicedge startedge, goaledge;	edge_ti graphstart, graphend; edge_ti ei_startEdge; edge_t ed_startEdge;
;
	tie(ei, eiend) = boost::edges(*g); //need this, will get error otherwise
	
	tie(graphstart, graphend) = boost::edges(*g);

	ed_startEdge = getRandomEdgeFromEdgeListIntegrated(g, graphstart, graphend, true);

	initEdge(ed_startEdge, startedge, *g);
	{
		displayEdgeV_i = startedge.start.index;
		displayEdgeV_ii = startedge.end.index;
	}

	std::vector<size_t>edgeinds;


	auto connedges = getConnectedEdges(g, startedge, cycs, edgeinds);

	int seconditer;	edge_t ed_goalEdge; edge_ti ei_goalEdge;
	if (connedges.size()) {


		updatetext(to_string(counter) + "t" + "[listlength" + to_string(connedges.size()) + "]");

		ed_goalEdge = getRandomEdgeFromEdgeListIntegrated(g, connedges.begin(), connedges.end());
		updatetext(to_string(counter) + "s");

		size_t cycleIndex = *(edgeinds.begin() + std::distance(connedges.begin(), std::find(connedges.begin(), connedges.end(), ed_goalEdge)));
		updatetext(to_string(counter) + "r");

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
		auto gei1 = std::distance(currCyc.begin(), find1);
		auto gei2 = std::distance(currCyc.begin(), find2);
		auto gei = (gei1 < gei2) ? find2 : find1;
		// get vetex indices
		updatetext(to_string(counter) + "d" + " (" + "values:" + to_string(goaledge.start.index) + ", " + to_string(goaledge.end.index) + ", " "indices" + to_string(gei1) + "," + to_string(gei2) + +";" + stringfromCyclesShort(currCyc) + ") ");


		vector<size_t> left(currCyc.begin(), gei);
		vector<size_t> right(gei, currCyc.end());
		updatetext(to_string(counter) + " (" + "left:" + stringfromCyclesShort(left) + ", right:" + stringfromCyclesShort(right) + ") ");
		updatetext(to_string(counter) + "e");
		startedge.divisionvert = boost::add_vertex(*g); fixedBool[startedge.divisionvert] = false;
		goaledge.divisionvert = boost::add_vertex(*g); fixedBool[goaledge.divisionvert] = false;
		//created 2 points
		//getDivPoint(startedge.descriptor);
		position[startedge.divisionvert] = interpolate(startedge, getDivPoint(startedge.descriptor));
		position[goaledge.divisionvert] = interpolate(goaledge, getDivPoint(startedge.descriptor));
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