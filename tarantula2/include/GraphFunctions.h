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



/**
 * removes item from vector by its index.
 *
 * @param reference to vector, index
 * @return nothing
 */
void removebyindex(vector<size_t>& vec, size_t ind) {
	vec.erase(std::remove(vec.begin(), vec.end(), ind), vec.end());
}
/**
 * connects goal- and startpoins, splits goal- and startedge;
 *
 * @param start and goal edge
 * @return vector containing all newly created edges
 */
vector<edge_t> connectEdges(Graph* g, cyclicedge startedge, cyclicedge goaledge, float rc) {
	vector<edge_t> returnedges(5);
	bool startforbidden=forbiddenPm[startedge.descriptor];
	bool goalforbidden= forbiddenPm[goaledge.descriptor];
	returnedges[0]=connectAB(g, startedge.divisionvert, goaledge.divisionvert, rc, startforbidden&&goalforbidden);
	returnedges[1] = connectAB(g, goaledge.divisionvert, goaledge.start.index, rc, indexPm[goaledge.descriptor], goalforbidden);
	returnedges[2] = connectAB(g, goaledge.divisionvert, goaledge.end.index, rc, indexPm[goaledge.descriptor], goalforbidden);
	returnedges[3] = connectAB(g, startedge.divisionvert, startedge.start.index, rc, indexPm[startedge.descriptor], startforbidden);
	returnedges[4] = connectAB(g, startedge.divisionvert, startedge.end.index, rc, indexPm[startedge.descriptor], startforbidden);
	fixedBool[goaledge.divisionvert] = goalforbidden;
	fixedBool[startedge.divisionvert] = startforbidden;
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
			else
				console() << "[error] vertex 1 and 2 are not adjacent" << endl;

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






std::vector<edge_t> getConnectableEdges(Graph* g, cyclicedge edge, std::vector<std::vector<size_t>>* cycs, std::vector<size_t>& edgeindices, bool forbCheck) {
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

						if ((!forbCheck||!forbiddenPm[cachedge]) && cachedge!=edge.descriptor) { //FORBIDDENCHECK ADDED
							availedges.push_back(cachedge);
							edgeindices.push_back(countr);
						}

						//if (cachedge != edge.descriptor) { //FORBIDDENCHECK ADDED
						//	availedges.push_back(cachedge);
						//	edgeindices.push_back(countr);
						//}
					}
				}

				
			}
		}
		countr++;
	}
	// TODO make clause for no edges, returning no edg

	return availedges;
}

tuple<vector<size_t>, vector<size_t>> divideCycleAtEdge(vector<size_t> cycle, cyclicedge edge) {
	auto find1 = std::find(cycle.begin(), cycle.end(), edge.start.index);
	auto find2 = std::find(cycle.begin(), cycle.end(), edge.end.index);
	auto gei1 = std::distance(cycle.begin(), find1);
	auto gei2 = std::distance(cycle.begin(), find2);
	auto gei = (gei1 < gei2) ? find2 : find1;
	// get vetex indices

	vector<size_t> left(cycle.begin(), gei);
	vector<size_t> right(gei, cycle.end());
	return std::make_tuple(left, right);
}

cyclicedge getStartEdge(Graph *g, bool b_excludeForbiddenEdges) {
	edge_ti graphstart, graphend;
	cyclicedge startedge;
	tie(graphstart, graphend) = boost::edges(*g);

	edge_t ed_startEdge = getRandomEdgeFromEdgeListIntegrated(g, graphstart, graphend, b_excludeForbiddenEdges);
	initEdge(ed_startEdge, startedge, *g);
	return startedge;
}
cyclicedge getGoalEdge(Graph *g, vector<edge_t> connectableEdges , bool b_excludeForbiddenEdges) {
	cyclicedge goaledge;
	edge_t ed_goalEdge = getRandomEdgeFromEdgeListIntegrated(g, connectableEdges.begin(), connectableEdges.end(), b_excludeForbiddenEdges); // also accept forbidden edges
	initEdge(ed_goalEdge, goaledge, *g);

	return goaledge;
}

void adjustGraphToNewEdges(Graph * g, std::vector<std::vector<size_t>>* cycs, cyclicedge& startedge,cyclicedge &goaledge,std::vector<edge_t> connectableEdges, std::vector<size_t>edgeinds) {

	size_t cycleIndex = *(edgeinds.begin() + std::distance(connectableEdges.begin(), std::find(connectableEdges.begin(), connectableEdges.end(), goaledge.descriptor)));
	vector<size_t> currCyc = cycs->at(cycleIndex);

	shiftCycle(currCyc, startedge.start.index, startedge.end.index);


	vector<size_t> left, right;
	tie(left, right) = divideCycleAtEdge(currCyc, goaledge);
	startedge.divisionvert = boost::add_vertex(*g); fixedBool[startedge.divisionvert] = false;
	goaledge.divisionvert = boost::add_vertex(*g); fixedBool[goaledge.divisionvert] = false;
	/*size_t cycleIndex = goaledge.cycles[0];*/

//created 2 points
//getDivPoint(startedge.descriptor);
	position[startedge.divisionvert] = interpolate(startedge, getDivPoint(startedge.descriptor));
	position[goaledge.divisionvert] = interpolate(goaledge, getDivPoint(goaledge.descriptor));
	if (forbiddenPm[startedge.descriptor]) {
		vec3 cp = getClosestPointFromList(position[goaledge.divisionvert], anchorPoints);
		console() << "closest point distance" << distance(cp, position[goaledge.divisionvert]) << endl;
		position[startedge.divisionvert] = cp;
	}
	if (forbiddenPm[goaledge.descriptor]) {
		vec3 cp = getClosestPointFromList(position[startedge.divisionvert], anchorPoints);
		console() << "closest point distance" << distance(cp, position[startedge.divisionvert]) << endl;
		position[goaledge.divisionvert] = cp;
	}
	//position[startedge.divisionvert] = interpolate(startedge, float((float(rand() % 5) + 1) / 6));
	cycs->at(cycleIndex) = left;
	cycs->push_back(right);
	size_t lastindex = cycs->size() - 1;

	resolveIntersections(startedge, goaledge, cycleIndex, lastindex, *g, *cycs);
}



void addRandomCyclicEdgeTesting(Graph* g, float rc, std::vector<std::vector<size_t>>* cycs) {

	// TODO FIX THE ISSUE WHEN THERE ARE NO AVAILABLE EDEGES
	std::vector<size_t>edgeinds;
	cyclicedge startedge= getStartEdge(g, CHECKFORBIDDEN);
	auto connectableEdges = getConnectableEdges(g, startedge, cycs, edgeinds, true);
	if (connectableEdges.size()) {
		cyclicedge goaledge = getGoalEdge(g, connectableEdges, false);
		adjustGraphToNewEdges(g, cycs, startedge, goaledge, connectableEdges, edgeinds);
		connectEdges(g, startedge, goaledge, rc);
	}
	else {
		console() << " no cycles found, returning" << endl;
		return;
	}

	counter++;
	console() << GLOBALINT << endl;
}

edge_t findRandomEdge() {
	edge_ti graphstart, graphend;
	tie(graphstart, graphend) = boost::edges(g);
	return getRandomEdgeFromEdgeListIntegrated(&g, graphstart,graphend, false);

}


void runxiterations(int x, Graph& g, float relaxc, std::vector<std::vector<size_t>>& cycles) {
	for (size_t i = 0; i < x; i++)
	{
		addRandomCyclicEdgeTesting(&g, relaxc, &cycles);
		for (size_t j = 0; j < 7; j++)
		{
			relaxPhysics(&g);
		}
		iterationcounter++;
	}

}